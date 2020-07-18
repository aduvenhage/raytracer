#ifndef LIBS_HEADER_MATERIAL_H
#define LIBS_HEADER_MATERIAL_H

#include "color.h"
#include "intersect.h"
#include "ray.h"

#include <random>


namespace LNF
{
    /* Diffuse material/texture base class */
    class Material
    {
     public:
        virtual ~Material() = default;
        
        /* Returns the diffuse color at the given surface position */
        virtual Color color(const Intersect &_hit) const = 0;
        
        /* Returns the emitted color at the given surface position */
        virtual Color emitted(const Intersect &_hit) const = 0;
        
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, const Ray &_ray, RandomGen &_randomGen) const = 0;
        
    };

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_MATERIAL_H

