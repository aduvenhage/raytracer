#ifndef CORE_MEMORY_H
#define CORE_MEMORY_H

#include "constants.h"

#include <cstdlib>
#include <memory>
#include <mutex>
#include <vector>
#include <unordered_map>


namespace CORE
{
    class MemoryManager
    {
     private:
        const static size_t     MEM_ALIGNMENT = sizeof(void*);
        const static size_t     MEM_POOL_SIZE = 1024 * 64;
        
     public:
        ~MemoryManager() {
            std::free(m_pMemory);
        }
        
        template <int POOL_INDEX>
        static MemoryManager &instance() {
            static MemoryManager instance(POOL_INDEX, MEM_POOL_SIZE);
            return instance;
        }
        
        void *allocate(size_t _uSize) {
            std::lock_guard<std::mutex> lock(m_mutex);
            size_t space = m_uTotalSize - ((size_t)m_pAllocPos - (size_t)m_pMemory);
            size_t size = _uSize + MEM_ALIGNMENT;
            
            char *pMem = (char*)std::align(MEM_ALIGNMENT, size, m_pAllocPos, space);
            assert(pMem != nullptr);
            m_pAllocPos = pMem + size;
            *((size_t*)pMem) = _uSize;

            return pMem + MEM_ALIGNMENT;
        }
        
        void deallocate(void *_pObj) {
            std::lock_guard<std::mutex> lock(m_mutex);
            
        }
        
        int id() const {
            return m_iId;
        }

     protected:
        MemoryManager(int _iId, size_t _n)
            :m_pMemory(nullptr),
             m_iId(_iId),
             m_uTotalSize(_n)
        {
            m_pMemory = std::malloc(m_uTotalSize);
            m_pAllocPos = m_pMemory;
        }
        
     private:
        void                                            *m_pMemory;
        void                                            *m_pAllocPos;
        int                                             m_iId;
        size_t                                          m_uTotalSize;
        std::mutex                                      m_mutex;
    };

};  // namespace CORE

#endif  // #ifndef CORE_MEMORY_H

