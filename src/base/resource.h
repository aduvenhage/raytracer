#pragma once

#include "core/memory.h"


namespace BASE
{
    class Resource
    {
     public:
        virtual ~Resource() = default;
        
        MANAGE_MEMORY('RSCN')
    };

};  // namespace BASE

