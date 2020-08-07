#ifndef LIBS_HEADER_MATERIAL_H
#define LIBS_HEADER_MATERIAL_H

#include "color.h"
#include "intersect.h"
#include "ray.h"


namespace LNF
{
    /* Material/texture base class */
    class Material
    {
     public:
        virtual ~Material() = default;
        
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, const Ray &_ray, RandomGen &_randomGen) const = 0;
    };

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_MATERIAL_H

