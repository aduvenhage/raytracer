#ifndef BASE_MATERIAL_H
#define BASE_MATERIAL_H

#include "core/color.h"
#include "core/ray.h"
#include "intersect.h"
#include "resource.h"


namespace BASE
{
    /* Material/texture base class */
    class Material      : public Resource
    {
     public:
        virtual ~Material() = default;
        
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay scatter(const Intersect &_hit) const = 0;
    };

};  // namespace BASE


#endif  // #ifndef BASE_MATERIAL_H

