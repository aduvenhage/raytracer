#ifndef BASE_RESOURCE_H
#define BASE_RESOURCE_H

#include "core/memory.h"


namespace BASE
{
    class Resource
    {
     public:
        virtual ~Resource() = default;
        
        MANAGE_MEMORY(MEM_POOL::SCENE)
    };

};  // namespace BASE

#endif // #ifndef BASE_RESOURCE_H
