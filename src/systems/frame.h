
#ifndef SYSTEMS_FRAME_H
#define SYSTEMS_FRAME_H



#include "core/constants.h"
#include "core/outputimage.h"
#include "core/viewport.h"
#include "core/ray.h"
#include "core/random.h"
#include "base/camera.h"
#include "base/scene.h"
#include "utils/jpeg.h"
#include "jobs.h"
#include "trace.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <atomic>



namespace SYSTEMS
{
    /* Gather and calculate frame stats */
    class FrameStats
    {
     private:
        using clock_type = std::chrono::high_resolution_clock;

     public:
        FrameStats()
            :m_uJobCount(0),
             m_fTotalJobProgress(0),
             m_uCompletedJobs(0),
             m_uRayCount(0),
             m_fTimeSpentS(0),
             m_fTimeToFinishS(0),
             m_fFrameProgress(0),
             m_fRaysPerSecond(0),
             m_bFinished(false)
        {
            m_tpStart = m_clock.now();
        }
        
        void setJobCount(size_t _uJobCount) {
            m_uJobCount = _uJobCount;
        }
        
        void setCompletedJobs(size_t _uCompletedJobs) {
            m_uCompletedJobs = _uCompletedJobs;
        }
        
        void setJobProgress(float _fTotalProgress) {
            m_fTotalJobProgress = _fTotalProgress;
        }
        
        void updateRayCount(uint64_t _uRayCountDelta) {
            m_uRayCount += _uRayCountDelta;
        }

        // recalculate frame stats
        void update() {
            if (m_bFinished == false) {
                auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(m_clock.now() - m_tpStart).count();
                m_fTimeSpentS = ns * 1e-9f;
            }

            m_fFrameProgress = m_fTotalJobProgress / m_uJobCount;
            if (m_fFrameProgress > 0.0001) {
                m_fTimeToFinishS = m_fTimeSpentS / m_fFrameProgress - m_fTimeSpentS;
            }

            if ( (m_fTimeSpentS > 1.0f) ||
                 (m_bFinished == true) )
            {
                m_fRaysPerSecond = m_uRayCount / m_fTimeSpentS;
            }

            m_bFinished = activeJobs() == 0;
        }
        
        size_t activeJobs() const {
            return m_uJobCount - m_uCompletedJobs;
        }
        
        float progress() const {
            return m_fFrameProgress;
        }
        
        float timeToFinish() const {
            return m_fTimeToFinishS;
        }
        
        float timeTotal() const {
            return m_fTimeSpentS;
        }
        
        float raysPerSecond() const {
            return m_fRaysPerSecond;
        }
        
        bool isFinished() const {
            return m_bFinished;
        }

     private:
        clock_type                              m_clock;
        clock_type::time_point                  m_tpStart;
        clock_type::time_point                  m_tpEnd;
        clock_type::time_point                  m_tpPerfCalc;
        size_t                                  m_uJobCount;
        
        std::atomic<float>                      m_fTotalJobProgress;
        std::atomic<size_t>                     m_uCompletedJobs;
        std::atomic<uint64_t>                   m_uRayCount;

        float                                   m_fTimeSpentS;
        float                                   m_fTimeToFinishS;
        float                                   m_fFrameProgress;
        float                                   m_fRaysPerSecond;
        bool                                    m_bFinished;
    };


    /* Raytracing job (line of pixels on output image) */
    class PixelJob  : public Job
    {
     public:
        PixelJob(const CORE::OutputImageBuffer *_pImage, int _iLine,
                 const CORE::Viewport *_pViewport,
                 const BASE::Camera *_pCamera,
                 const BASE::Scene *_pScene,
                 FrameStats *_pFrameStats,
                 int _iMaxSamplesPerPixel,
                 int _iMaxDepth)
            :m_pImage(_pImage),
             m_pViewport(_pViewport),
             m_pCamera(_pCamera),
             m_pScene(_pScene),
             m_pFrameStats(_pFrameStats),
             m_iLine(_iLine),
             m_iMaxSamplesPerPixel(_iMaxSamplesPerPixel),
             m_iMaxDepth(_iMaxDepth),
             m_fProgress(0)
        {}
        
        // do the work -- blocks until completed
        virtual void run() override
        {
            RayTracer tracer(m_pScene, m_iMaxDepth);
            const float fFovScale = tan(m_pCamera->fov() * 0.5f);
            unsigned char *pPixel = (unsigned char *)m_pImage->row(m_iLine);
            
            // run through one line in output image
            const float y = (1.0f - 2.0f * m_iLine / m_pViewport->height()) * fFovScale;

            for (auto i = 0; i < m_pViewport->width(); i++)
            {
                const float x = (1.0f - 2.0f * i / m_pViewport->width()) * fFovScale * m_pViewport->viewAspect();
                CORE::Color color;
                int n = 0;
                
                auto stats = CORE::ColorStat();
                for (int k = 0; k < m_iMaxSamplesPerPixel; k++)
                {
                    // calc origin in camera
                    auto rayOrigin = CORE::randomInUnitDisc() * m_pCamera->aperture() * 0.5;
                    
                    // calc lookat point on focus plane
                    auto rayFocus = (CORE::Vec(x, y, 1) + randomInPixel()) * m_pCamera->focusDistance();
                    
                    // create ray (transform from camera to world)
                    rayOrigin = m_pCamera->axis().transformFrom(rayOrigin);
                    rayFocus = m_pCamera->axis().transformFrom(rayFocus);
                    auto ray = CORE::Ray(rayOrigin, (rayFocus - rayOrigin).normalized(), true);
                    
                    // trace ray
                    color += tracer.trace(ray);
                    n++;
                }
                                
                // get pixel color
                color = (color/n).clamp().gammaCorrect2();

                // write to output image
                *(pPixel++) = (int)(255 * color.red() + 0.5);
                *(pPixel++) = (int)(255 * color.green() + 0.5);
                *(pPixel++) = (int)(255 * color.blue() + 0.5);
                
                m_fProgress =  (float)i / m_pViewport->width();
            }

            // update frame stats
            m_pFrameStats->updateRayCount(tracer.rayCount());
        }

        // returns progress [0..1] while the job is running
        virtual float progress() const override {
            return m_fProgress;
        }

     private:
        CORE::Vec randomInPixel() const {
            CORE::Vec ret = CORE::randomInUnitSquare();
            ret.x() *= 0.5 / m_pViewport->width();
            ret.y() *= 0.5 / m_pViewport->height();
            return ret;
        }

     private:
        const CORE::OutputImageBuffer  *m_pImage;
        const CORE::Viewport           *m_pViewport;
        const BASE::Camera             *m_pCamera;
        const BASE::Scene              *m_pScene;
        FrameStats                     *m_pFrameStats;
        int                            m_iLine;
        int                            m_iMaxSamplesPerPixel;
        int                            m_iMaxDepth;
        std::atomic<float>             m_fProgress;
    };


    /* Raytracing worker (with random seeding) */
    class PixelWorker   : public Worker
    {
     public:
        PixelWorker(JobQueue *_pJobs, int _iJobChunkSize, uint32_t _uRandSeed)
            :Worker(_pJobs, _iJobChunkSize),
             m_uRandomSeed(_uRandSeed)
        {}
        
     private:
        virtual void onStart() override {
            CORE::seed(m_uRandomSeed);
        }
        
     private:
        uint32_t    m_uRandomSeed;
    };

    
    /*
     Container for output image and job system for a single frame
     */
    class Frame
    {
     protected:
        const static int    JOB_CHUNK_SIZE      = 4;      // number of jobs grabbed by worker

     public:
        Frame(int _iWidth, int _iHeight,
              const BASE::Camera *_pCamera,
              const BASE::Scene *_pScene,
              int _iNumWorkers,
              int _iMaxSamplesPerPixel,
              int _iMaxTraceDepth,
              uint32_t _uRandSeed)
            :m_viewport(_iWidth, _iHeight),
             m_pCamera(_pCamera),
             m_pScene(_pScene),
             m_image(_iWidth, _iHeight),
             m_iMaxSamplesPerPixel(_iMaxSamplesPerPixel),
             m_iNumWorkers(_iNumWorkers),
             m_iMaxTraceDepth(_iMaxTraceDepth),
             m_uRandomSeed(_uRandSeed)
        {
            CORE::generator().seed(m_uRandomSeed);

            createJobs();
            createWorkers();
        }
        
        virtual ~Frame() {
            // stop all workers and then wait for them to finish
            for (const auto &pWorker : m_workers) {
                pWorker->stop();
            }
            
            m_workers.clear();
        }
        
        void updateFrameProgress() {
            // calc active jobs and progress
            int completedJobs = 0;
            float totalProgress = 0.0f;
            for (const auto &pWorker : m_workers) {
                completedJobs += pWorker->completedJobs();
                totalProgress += pWorker->totalProgress();
            }

            // update stats
            m_frameStats.setJobProgress(totalProgress);
            m_frameStats.setCompletedJobs(completedJobs);
            m_frameStats.update();
        }
        
        size_t activeJobs() const {
            return m_frameStats.activeJobs();
        }
        
        float progress() const {
            return m_frameStats.progress();
        }
        
        float timeToFinish() const {
            return m_frameStats.timeToFinish();
        }
        
        float timeTotal() const {
            return m_frameStats.timeTotal();
        }
        
        float raysPerSecond() const {
            return m_frameStats.raysPerSecond();
        }
        
        bool isFinished() const {
            return m_frameStats.isFinished();
        }
        
        // write current image to file
        int writeJpegFile(const std::string &_strPath, int _iQuality)
        {
            return UTILS::writeJpegFile(_strPath.c_str(), m_image.width(), m_image.height(), m_image.data(), _iQuality);
        }
        
        CORE::OutputImageBuffer &image() {
            return m_image;
        }
        
     private:
        // split output image into pixel jobs
        void createJobs() {
            // create jobs
            std::vector<std::unique_ptr<Job>> jobs;
            for (int j = 0; j < m_image.height(); j++) {
                jobs.push_back(std::make_unique<PixelJob>(&m_image, j,
                                                          &m_viewport,
                                                          m_pCamera,
                                                          m_pScene,
                                                          &m_frameStats,
                                                          m_iMaxSamplesPerPixel,
                                                          m_iMaxTraceDepth));
            }
            
            m_frameStats.setJobCount(jobs.size());
            
            // shuffle jobs a little
            m_jobQueue.push_shuffle(jobs, CORE::generator());
        }
        
        // create worker threads
        void createWorkers() {
            std::vector<std::unique_ptr<Worker>> workers;
            for (int i = 0; i < m_iNumWorkers; i++) {
                m_workers.push_back(std::make_unique<PixelWorker>(&m_jobQueue, (int)JOB_CHUNK_SIZE, m_uRandomSeed));
            }
        }
        
     private:
        const CORE::Viewport                       m_viewport;
        const BASE::Camera                         *m_pCamera;
        const BASE::Scene                          *m_pScene;
        JobQueue                                   m_jobQueue;
        std::vector<std::unique_ptr<Worker>>       m_workers;
        CORE::OutputImageBuffer                    m_image;
        FrameStats                                 m_frameStats;
        int                                        m_iMaxSamplesPerPixel;
        int                                        m_iNumWorkers;
        int                                        m_iMaxTraceDepth;
        uint32_t                                   m_uRandomSeed;
    };
    
    
};  // namespace SYSTEMS


#endif  // #ifndef SYSTEMS_FRAME_H
