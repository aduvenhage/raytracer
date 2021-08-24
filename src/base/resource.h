#ifndef BASE_RESOURCE_H
#define BASE_RESOURCE_H

#include "core/memory.h"


namespace BASE
{
    class Resource
    {
     public:
        virtual ~Resource() = default;
        
        void *operator new(size_t _uSize) {
            return CORE::MemoryManager::instance<1>().allocate(_uSize);
        }
        
        void operator delete(void *_p) {
            return CORE::MemoryManager::instance<1>().deallocate(_p);
        }
 
    };

};  // namespace BASE

#endif // #ifndef BASE_RESOURCE_H
