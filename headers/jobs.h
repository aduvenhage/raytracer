#ifndef LIBS_HEADER_JOBS_H
#define LIBS_HEADER_JOBS_H

#include "constants.h"

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
    class JobQueue
    {
     public:
        JobQueue()
            :m_iSize(0)
        {}
        
        virtual ~JobQueue() = default;
        
        /* returns true if no more jobs */
        virtual bool empty() const {
            return m_iSize == 0;
        }
        
        /* returns true if no more jobs */
        virtual size_t size() const {
            return m_iSize;
        }
        
        /* add job to pool */
        virtual void push(std::unique_ptr<Job> &&_pJob) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_jobs.push(std::move(_pJob));
            m_iSize = (int)m_jobs.size();
        }
        
        /* take jobs from pool */
        virtual std::vector<std::unique_ptr<Job>> pop(size_t _uNumJobs) {
            std::vector<std::unique_ptr<Job>> ret;
            ret.reserve(_uNumJobs);
            
            std::lock_guard<std::mutex> lock(m_mutex);
            int count = 0;
            while (m_jobs.empty() == false) {
                ret.push_back(std::move(m_jobs.front()));
                m_jobs.pop();
                m_iSize = (int)m_jobs.size();

                if (++count >= _uNumJobs) {
                    break;
                }
            }
            
            return ret;
        }
        
     protected:
        std::queue<std::unique_ptr<Job>>    m_jobs;
        std::atomic<int>                    m_iSize;
        mutable std::mutex                  m_mutex;
    };


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
                    m_iActiveJobs = (int)m_localJobs.size();

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
