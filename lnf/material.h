#ifndef LIBS_HEADER_MATERIAL_H
#define LIBS_HEADER_MATERIAL_H

#include "color.h"
#include "intersect.h"
#include "ray.h"
#include "resource.h"


namespace LNF
{
    /* Material/texture base class */
    class Material      : public Resource
    {
     public:
        virtual ~Material() = default;
        
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, RandomGen &_randomGen) const = 0;
    };

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_MATERIAL_H

