#ifndef SYSTEMS_JOBS_H
#define SYSTEMS_JOBS_H

#include "core/constants.h"
#include "core/memory.h"
#include "core/queue.h"

#include <atomic>
#include <memory>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>


namespace SYSTEMS
{
    /* Job -- generic piece of work for a worker */
    class Job
    {
     public:
        MANAGE_MEMORY(MEM_POOL::JOB_SYSTEM)
        virtual ~Job() = default;
        
        // do the work -- blocks until completed
        virtual void run() = 0;
        
        // returns progress [0..1] while the job is running
        virtual float progress() const = 0;
    };


    /* collection of jobs for workers */
    using JobQueue = CORE::Queue<std::unique_ptr<Job>>;


    /* Worker that can execute jobs */
    class Worker
    {
     public:
        MANAGE_MEMORY(MEM_POOL::JOB_SYSTEM)
        Worker(JobQueue *_pJobs, int _iJobChunkSize)
            :m_pJobs(_pJobs),
             m_iJobChunkSize(_iJobChunkSize),
             m_iActiveJobs(0),
             m_iCompletedJobs(0),
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
        
        /* return the number of jobs completed successfully */
        virtual int completedJobs() const {
            return m_iCompletedJobs;
        }
        
        /* returns sum of progress of all jobs */
        virtual float totalProgress() const {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_pCurrentJob != nullptr) {
                return (float)m_iCompletedJobs + m_pCurrentJob->progress();
            }
            else {
                return (float)m_iCompletedJobs;
            }
        }
        
     private:
        virtual void onStart() {}
        virtual void onFinished() {}

     private:
        // thread entry point
        void run() {
            onStart();

            while (m_bRunning == true) {
                // grab new jobs if local list is empty
                if (m_localJobs.empty() == true) {
                    m_localJobs = m_pJobs->pop(m_iJobChunkSize);
                    m_iActiveJobs = (int)m_localJobs.size();
                }

                // work on first job in local list
                if (m_localJobs.empty() == false) {
                    // find next job
                    m_iActiveJobs = (int)m_localJobs.size();
                    m_pCurrentJob = std::move(m_localJobs.back());
                    m_localJobs.pop_back();

                    // run job
                    m_pCurrentJob->run();

                    // cleanup (sync changes for correct progress reporting)
                    std::lock_guard<std::mutex> lock(m_mutex);
                    m_pCurrentJob = nullptr;
                    m_iCompletedJobs++;
                    m_iActiveJobs--;
                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
            
            onFinished();
        }
        
     private:
        std::thread                                 m_thread;
        JobQueue                                    *m_pJobs;
        int                                         m_iJobChunkSize;
        std::vector<std::unique_ptr<Job>>           m_localJobs;
        mutable std::mutex                          m_mutex;
        std::unique_ptr<Job>                        m_pCurrentJob;
        std::atomic<int>                            m_iActiveJobs;
        std::atomic<int>                            m_iCompletedJobs;
        std::atomic<bool>                           m_bRunning;
    };


};  // namespace SYSTEMS


#endif  // #ifndef SYSTEMS_JOBS_H
