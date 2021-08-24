
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
            :m_uActiveJobs(0),
             m_uJobCount(0),
             m_uRayCount(0),
             m_fFrameProgress(0),
             m_fTimeSpentS(0),
             m_fTimeToFinishS(0),
             m_fRaysPerSecond(0),
             m_bFinished(false),
             m_bUpdates(false)
        {
            m_tpStart = m_clock.now();
        }
        
        void setJobCount(size_t _uJobCount) {
            m_uJobCount = _uJobCount;
        }
        
        void setActiveJobs(size_t _uActiveJobs) {
            if (m_uActiveJobs != _uActiveJobs) {
                m_uActiveJobs = _uActiveJobs;
                m_bUpdates = true;
            }
        }
        
        void updateRayCount(uint64_t _uRayCountDelta) {
            if (_uRayCountDelta > 0) {
                m_uRayCount += _uRayCountDelta;
                m_bUpdates = true;
            }
        }

        // recalculate frame stats
        bool update() {
            if (m_bUpdates == true) {
                // calc time spent
                auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(m_clock.now() - m_tpStart).count();
                m_fTimeSpentS = ns * 1e-9f;

                // calc perf, progress and time to go
                m_fFrameProgress = (float)(m_uJobCount - m_uActiveJobs) / m_uJobCount;
                m_bFinished = m_uActiveJobs == 0;
                
                if (m_fFrameProgress > 0.0001) {
                    m_fTimeToFinishS = m_fTimeSpentS / m_fFrameProgress - m_fTimeSpentS;
                }

                if ( (m_fTimeSpentS > 1.0f) ||
                     (m_bFinished == true) )
                {
                    m_fRaysPerSecond = m_uRayCount / m_fTimeSpentS;
                }
                
                m_bUpdates = false;
                return true;
            }
            
            return false;
        }
        
        size_t activeJobs() const {
            return m_uActiveJobs;
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
        
        size_t                                  m_uActiveJobs;
        size_t                                  m_uJobCount;
        std::atomic<uint64_t>                   m_uRayCount;

        float                                   m_fFrameProgress;
        float                                   m_fTimeSpentS;
        float                                   m_fTimeToFinishS;
        float                                   m_fRaysPerSecond;
        bool                                    m_bFinished;
        bool                                    m_bUpdates;
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
                 int _iMaxDepth,
                 float _fColorTollerance)
            :m_pImage(_pImage),
             m_pViewport(_pViewport),
             m_pCamera(_pCamera),
             m_pScene(_pScene),
             m_pFrameStats(_pFrameStats),
             m_iLine(_iLine),
             m_iMaxSamplesPerPixel(_iMaxSamplesPerPixel),
             m_iMaxDepth(_iMaxDepth),
             m_fColorTollerance(_fColorTollerance)
        {}
        
        void run()
        {
            RayTracer tracer(m_pScene, m_iMaxDepth);
            unsigned char *pPixel = (unsigned char *)m_pImage->row(m_iLine);
            const int iViewWidth = m_pViewport->width();
            const int iViewHeight = m_pViewport->height();
            const int iMaxSamplesPerPixel = m_iMaxSamplesPerPixel;
            const float fViewAspect = m_pViewport->viewAspect();
            const float fFov = m_pCamera->fov();
            const float fFovScale = tan(fFov * 0.5f);
            const float fCameraAperature = m_pCamera->aperture();
            const float fCameraFocusDistance = m_pCamera->focusDistance();
            const float fSubPixelScale = 0.5f / iViewWidth;
            const float y = (1 - 2 * m_iLine / (float)iViewHeight) * fFovScale;
            const float fColorTollerance = m_fColorTollerance;
            const CORE::Axis &axisCameraView = m_pCamera->axis();

            for (auto i = 0; i < iViewWidth; i++)
            {
                const float x = (2 * i / (float)iViewWidth - 1) * fViewAspect * fFovScale;
                
                auto stats = CORE::ColorStat();
                for (int k = 0; k < iMaxSamplesPerPixel; k++)
                {
                    // set ray depth of field and focus aliasing
                    auto rayOrigin = CORE::randomUnitDisc() * fCameraAperature * 0.5;
                    auto rayFocus = (CORE::randomUnitSquare() * fSubPixelScale + CORE::Vec(-x, y, 1)).normalized() * fCameraFocusDistance;
                    
                    // transform from camera to world
                    rayOrigin = axisCameraView.transformFrom(rayOrigin);
                    rayFocus = axisCameraView.transformFrom(rayFocus);
                    
                    // create ray
                    auto ray = CORE::Ray(rayOrigin, (rayFocus - rayOrigin).normalized());
                    
                    // trace ray
                    auto color = tracer.trace(ray);
                    stats.push(color);
                    
                    // check color stats for a quick exit
                    if ( (fColorTollerance > 0.0f) &&
                         (k >= 16) &&
                         (stats.standardDeviation() < fColorTollerance) )
                    {
                        break;
                    }
                }
                                
                // write averaged color to output image
                auto color = stats.mean();
                color.clamp();

                *(pPixel++) = (int)(255 * color.red() + 0.5);
                *(pPixel++) = (int)(255 * color.green() + 0.5);
                *(pPixel++) = (int)(255 * color.blue() + 0.5);
            }

            // update frame stats
            m_pFrameStats->updateRayCount(tracer.rayCount());
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
        float                          m_fColorTollerance;
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
        Frame(const CORE::Viewport *_pViewport,
              const BASE::Camera *_pCamera,
              const BASE::Scene *_pScene,
              int _iNumWorkers,
              int _iMaxSamplesPerPixel,
              int _iMaxTraceDepth,
              float _fColorTollerance,
              uint32_t _uRandSeed)
            :m_pViewport(_pViewport),
             m_pCamera(_pCamera),
             m_pScene(_pScene),
             m_uJobCount(0),
             m_image(_pViewport->width(), _pViewport->height()),
             m_iMaxSamplesPerPixel(_iMaxSamplesPerPixel),
             m_iNumWorkers(_iNumWorkers),
             m_iMaxTraceDepth(_iMaxTraceDepth),
             m_fColorTollerance(_fColorTollerance),
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
        
        bool updateFrameProgress() {
            // calc active jobs
            auto completedJobs = 0;
            for (const auto &pWorker : m_workers) {
                completedJobs += pWorker->completedJobs();
            }
            
            int activeJobs = (int)m_uJobCount - completedJobs;
            if (activeJobs < 0) {
                activeJobs = 0;
            }

            // update stats
            m_frameStats.setActiveJobs(activeJobs);
            return m_frameStats.update();
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
                                                           m_pViewport,
                                                           m_pCamera,
                                                           m_pScene,
                                                           &m_frameStats,
                                                           m_iMaxSamplesPerPixel,
                                                           m_iMaxTraceDepth,
                                                           m_fColorTollerance));
                m_uJobCount++;
            }
            
            m_frameStats.setJobCount(m_uJobCount);
            
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
        const CORE::Viewport                       *m_pViewport;
        const BASE::Camera                         *m_pCamera;
        const BASE::Scene                          *m_pScene;
        size_t                                     m_uJobCount;
        JobQueue                                   m_jobQueue;
        std::vector<std::unique_ptr<Worker>>       m_workers;
        CORE::OutputImageBuffer                    m_image;
        FrameStats                                 m_frameStats;
        int                                        m_iMaxSamplesPerPixel;
        int                                        m_iNumWorkers;
        int                                        m_iMaxTraceDepth;
        float                                      m_fColorTollerance;
        uint32_t                                   m_uRandomSeed;
    };
    
    
};  // namespace SYSTEMS


#endif  // #ifndef SYSTEMS_FRAME_H
