#ifndef LIBS_HEADER_JOBS_H
#define LIBS_HEADER_JOBS_H

#include "constants.h"
#include "queue.h"

#include <atomic>
#include <memory>
#include <thread>
#include <queue>
#include <vector>



namespace LNF
{

    /* Job -- generic piece of work for a worker */
    class Job
    {
     public:
        virtual ~Job() = default;
        
        virtual void run() = 0;
    };


    /* collection of jobs for workers */
    using JobQueue = Queue<std::unique_ptr<Job>>;


    /* Worker that can execute jobs */
    class Worker
    {
     public:
        Worker(JobQueue *_pJobs, int _iJobChunkSize)
            :m_pJobs(_pJobs),
             m_iJobChunkSize(_iJobChunkSize),
             m_iActiveJobs(0),
             m_bRunning(true)
        {
            m_thread = std::thread(&Worker::run, this);
        }
        
        virtual ~Worker() {
            stop();
            if (m_thread.joinable() == true) {
                m_thread.join();
            }
        }
        
        virtual void stop() {
            m_bRunning = false;
        }
        
        virtual bool running() const {
            return m_bRunning;
        }

        /* returns true if worker has local jobs */
        virtual bool busy() const {
            return m_iActiveJobs > 0;
        }
        
        /* returns the number of local jobs */
        virtual int activeJobs() const {
            return m_iActiveJobs;
        }

     private:
        // thread entry point
        void run() {
            while (m_bRunning == true) {
                // grab new jobs if local list is empty
                if (m_localJobs.empty() == true) {
                    m_localJobs = m_pJobs->pop(m_iJobChunkSize);
                    m_iActiveJobs = (int)m_localJobs.size();
                }

                // work on first job in local list
                if (m_localJobs.empty() == false) {
                    auto pJobPtr = std::move(m_localJobs.back());
                    m_localJobs.pop_back();
                    m_iActiveJobs = (int)m_localJobs.size() + 1;

                    pJobPtr->run();
                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        }
        
     protected:
        std::thread                                 m_thread;
        JobQueue                                    *m_pJobs;
        int                                         m_iJobChunkSize;
        std::vector<std::unique_ptr<Job>>           m_localJobs;
        std::atomic<int>                            m_iActiveJobs;
        std::atomic<bool>                           m_bRunning;
    };


};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_JOBS_H
