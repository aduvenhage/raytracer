
#ifndef LIBS_HEADER_QUEUE_H
#define LIBS_HEADER_QUEUE_H

#include "constants.h"

#include <atomic>
#include <thread>
#include <queue>


namespace LNF
{

    /*
        Simple thread-safe queue.
        Uses lock internally and can handle multiple-producers and consumers.
        Push and pop do not block, except while waiting for locks.
    */
    template <typename item_type>
    class Queue
    {
     public:
        Queue()
            :m_iSize(0)
        {}
        
        ~Queue() = default;
        
        bool empty() const {
            return m_iSize == 0;
        }
        
        size_t size() const {
            return m_iSize;
        }
        
        template <typename T>
        void push(T &&_item) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(std::forward(_item));
            m_iSize = (int)m_queue.size();
        }
        
        void push(std::vector<item_type> &_items) {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (auto &item : _items) {
                m_queue.push(std::move(item));
            }
            
            m_iSize = (int)m_queue.size();
            _items.clear();
        }
        
        template <typename random_gen>
        void push_shuffle(std::vector<item_type> &_items, random_gen &_randomGen) {
            std::shuffle(_items.begin(), _items.end(), _randomGen);
            push(_items);
        }

        std::vector<item_type> pop(size_t _n) {
            std::vector<item_type> ret;
            ret.reserve(_n);
            
            std::lock_guard<std::mutex> lock(m_mutex);
            int count = 0;
            while (m_queue.empty() == false) {
                ret.push_back(std::move(m_queue.front()));
                m_queue.pop();

                if (++count >= (int)_n) {
                    break;
                }
            }
            
            m_iSize = (int)m_queue.size();
            
            return ret;
        }
        
     protected:
        std::queue<item_type>       m_queue;
        std::atomic<int>            m_iSize;
        mutable std::mutex          m_mutex;
    };
    
    
    
    /* Ring buffer based queue (lock free and thread-safe for single producer / single consumer) */
    template <typename payload_type, int N>
    class LockFreeQueue
    {
     static_assert(isPowerOf2(N), "Queue internal size should be a power of two.");
     protected:
        const size_t    MASK = N-1;
        
     public:
        LockFreeQueue()
            :m_uFront(0),
             m_uBack(0)
        {}
        
        template <typename T>
        bool push(T &&_p) {
            if (full() == true) {
                return false;
            }
            
            m_array[m_uBack & MASK] = std::forward<T>(_p);
            m_uBack++;
            return true;
        }
        
        payload_type &back() {
            return m_array[(m_uBack-1) & MASK];
        }
        
        bool pop(payload_type &_p) {
            if (empty() == true) {
                return false;
            }
            
            _p = std::move(m_array[m_uFront & MASK]);
            m_uFront++;
            return true;
        }
        
        payload_type pop() {
            if (empty() == true) {
                return payload_type();
            }
            
            auto p = std::move(m_array[m_uFront & MASK]);
            m_uFront++;
            return p;
        }
        
        bool empty() const {
            return m_uBack == m_uFront;
        }
        
        bool full() const {
            return m_uBack - m_uFront >= N;
        }
        
        size_t size() const {
            return m_uBack - m_uFront;
        }
        
     private:
        std::array<payload_type, N>    m_array;
        std::atomic<uint32_t>          m_uFront;
        std::atomic<uint32_t>          m_uBack;
    };
    
    
};  // namespace LNF


#endif // #ifndef LIBS_HEADER_QUEUE_H
