
#ifndef LIBS_HEADER_FRAME_H
#define LIBS_HEADER_FRAME_H



#include "constants.h"
#include "jobs.h"
#include "jpeg.h"
#include "outputimage.h"
#include "viewport.h"
#include "scene.h"
#include "trace.h"

#include <algorithm>


namespace LNF
{
    /* Raytracing job (block of pixels on output image) */
    class PixelJob  : public Job
    {
     public:
        PixelJob(OutputImageBuffer *_pImage, const Viewport *_pViewport,
                 int _i, int _j, int _iBlockWidth, int _iBlockHeight,
                 const Scene *_pScene,
                 int _iRaysPerPixel,
                 int _iMaxDepth,
                 float _fColorTollerance)
            :m_output(_pImage, _i, _j, _iBlockWidth, _iBlockHeight),
             m_view(_pViewport, _i, _j),
             m_pScene(_pScene),
             m_iRaysPerPixel(_iRaysPerPixel),
             m_iMaxDepth(_iMaxDepth),
             m_fColorTollerance(_fColorTollerance)
        {}
        
        void run()
        {
            thread_local static RandomGen generator;
            rayTraceImage(&m_output, &m_view, m_pScene, generator, m_iRaysPerPixel, m_iMaxDepth, m_fColorTollerance);
        }

     private:
        OutputImageBlock               m_output;
        ViewportBlock                  m_view;
        const Scene                    *m_pScene;
        int                            m_iRaysPerPixel;
        int                            m_iMaxDepth;
        float                          m_fColorTollerance;
    };

    
    /* Container for output image and job system for a single frame */
    class Frame
    {
     protected:
        const static int    PIXEL_BLOCK_SIZE    = 16;     // size of pixel blocks jobs work on
        const static int    JOB_CHUNK_SIZE      = 4;      // number of jobs grabbed by worker
        
     public:
        Frame(const ViewportScreen *_pViewport,
              const Scene *_pScene,
              int _iNumWorkers,
              int _iSamplesPerPixel,
              int _iMaxTraceDepth,
              float _fColorTollerance)
            :m_pViewport(_pViewport),
             m_pScene(_pScene),
             m_image(_pViewport->width(), _pViewport->height()),
             m_iPixelBlockSize(PIXEL_BLOCK_SIZE),
             m_iSamplesPerPixel(_iSamplesPerPixel),
             m_iNumWorkers(_iNumWorkers),
             m_iMaxTraceDepth(_iMaxTraceDepth),
             m_fColorTollerance(_fColorTollerance)
        {
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
        
        int activeJobs() const {
            int numJobs = (int)m_jobQueue.size();
            for (const auto &pWorker : m_workers) {
                numJobs += pWorker->activeJobs();
            }
                
            return numJobs;
        }
        
        bool isFinished() const {
            return activeJobs() == 0;
        }
        
        // write current image to file
        int writeJpegFile(const std::string &_strPath, int _iQuality)
        {
            return LNF::writeJpegFile(_strPath.c_str(), m_image.width(), m_image.height(), m_image.data(), _iQuality);
        }
        
        OutputImageBuffer &image() {
            return m_image;
        }
        
     private:
        // split output image into pixel jobs
        void createJobs() {
            // chop output image into smaller blocks
            int width = m_image.width();
            int height = m_image.height();
            std::vector<std::unique_ptr<Job>> jobs;

            for (int j = 0; j < height; j += m_iPixelBlockSize) {
                int iBlockHeight = m_iPixelBlockSize;
                if (iBlockHeight > height - j) {
                    iBlockHeight = height - j;
                }

                for (int i = 0; i < width; i += m_iPixelBlockSize) {
                    int iBlockWidth = m_iPixelBlockSize;
                    if (iBlockWidth > width - i) {
                        iBlockWidth = width - i;
                    }
                    
                    // create a job per block
                    jobs.push_back(std::make_unique<PixelJob>(&m_image, m_pViewport,
                                                               i, j, iBlockWidth, iBlockHeight,
                                                               m_pScene,
                                                               m_iSamplesPerPixel,
                                                               m_iMaxTraceDepth,
                                                               m_fColorTollerance));
                }
            }
            
            // shuffle jobs a little
            m_jobQueue.push_shuffle(jobs, m_generator);
        }
        
        // create worker threads
        void createWorkers() {
            std::vector<std::unique_ptr<Worker>> workers;
            for (int i = 0; i < m_iNumWorkers; i++) {
                m_workers.push_back(std::make_unique<Worker>(&m_jobQueue, (int)JOB_CHUNK_SIZE));
            }
        }
        
     private:
        const ViewportScreen                    *m_pViewport;
        const Scene                             *m_pScene;
        JobQueue                                m_jobQueue;
        std::vector<std::unique_ptr<Worker>>    m_workers;
        OutputImageBuffer                       m_image;
        RandomGen                               m_generator;
        int                                     m_iPixelBlockSize;
        int                                     m_iSamplesPerPixel;
        int                                     m_iNumWorkers;
        int                                     m_iMaxTraceDepth;
        float                                   m_fColorTollerance;
    };
    
    
};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_FRAME_H
