#ifndef CORE_MEMORY_H
#define CORE_MEMORY_H

#include "constants.h"

#include <cassert>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <vector>
#include <unordered_map>


namespace CORE
{
    /*
        Memory manager, making all allocations in one big block.
        Deleted memory is placed in free lists (sorted by size) for quick allocation again.
     */
    class MemoryManager
    {
     private:
        const static size_t     DEFAULT_POOL_SIZE = 1024 * 256;     // 256KB
        
     public:
        ~MemoryManager() {
            std::free(m_pMemory);
        }
        
        template <int POOL_INDEX>
        static MemoryManager &instance() {
            static MemoryManager instance(POOL_INDEX, DEFAULT_POOL_SIZE);
            return instance;
        }
        
        void *allocate(size_t _n) {
            // allocate block size rounded up to multiple of 8 and with an extra 8 bytes to store block size
            size_t size = align64(_n) + 8;
            unsigned char *p = allocateFromDeleted(size);
            if (p == nullptr) {
                p = allocateNew(size);
            }

            // store block size at start of block
            *((size_t*)p) = size;
            return p + 8;
        }
        
        void deallocate(void *_p) {
            // find block size
            unsigned char *pMem = (unsigned char*)_p - 8;
            size_t size = *((size_t*)pMem);
            
            // put block in free lists
            std::lock_guard<std::mutex> lock(m_mutex);
            m_freeLists[size].push_back(pMem);
        }
        
        int id() const {
            return m_iId;
        }

     private:
        MemoryManager(int _iId, size_t _n)
            :m_pMemory(nullptr),
             m_iId(_iId),
             m_uTotalSize(_n)
        {
            m_pMemory = (unsigned char*)std::malloc(m_uTotalSize);
            m_pAllocPos = (unsigned char*)align_ptr(m_pMemory);
        }
        
        uintptr_t align64(uintptr_t _n) {
            return (_n + 7) & (-8);
        }

        unsigned char *align_ptr(void *_p) {
            return (unsigned char*)align64((uintptr_t)_p);
        }
        
        unsigned char *allocateFromDeleted(size_t _n) {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto &freeList = m_freeLists[_n];
            if (freeList.empty() == false) {
                unsigned char *p = freeList.back();
                freeList.pop_back();
                return p;
            }
            
            return nullptr;
        }

        unsigned char *allocateNew(size_t _n) {
            std::lock_guard<std::mutex> lock(m_mutex);
            assert(m_pAllocPos + _n < m_pMemory + m_uTotalSize);
            unsigned char *p = m_pAllocPos;
            m_pAllocPos += _n;
            return p;
        }

     private:
        unsigned char                                           *m_pMemory;
        unsigned char                                           *m_pAllocPos;
        int                                                     m_iId;
        size_t                                                  m_uTotalSize;
        std::mutex                                              m_mutex;
        std::unordered_map<size_t, std::vector<unsigned char*>> m_freeLists;
    };


};  // namespace CORE


#define USE_MEMORY_POOLS    0


// add to a class to overload memory operators
#if USE_MEMORY_POOLS != 0
    #define MANAGE_MEMORY(poolIndex)                                                \
        void *operator new(size_t _uSize) {                                         \
            return CORE::MemoryManager::instance<(int)poolIndex>().allocate(_uSize);\
        }                                                                           \
        void operator delete(void *_p) {                                            \
            return CORE::MemoryManager::instance<(int)poolIndex>().deallocate(_p);  \
        }                                                                           \
        
#else
    #define MANAGE_MEMORY(poolIndex)
#endif

// some default memory manager IDs
enum class MEM_POOL {
    SCENE = 1,
    JOB_SYSTEM = 2
};

#endif  // #ifndef CORE_MEMORY_H

