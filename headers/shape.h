#ifndef LIBS_HEADER_SHAPE_H
#define LIBS_HEADER_SHAPE_H

#include "constants.h"
#include "intersect.h"
#include "material.h"
#include "ray.h"

#include <memory>


namespace LNF
{
    /* Base class for all raytraced Shapes */
    class Shape
    {
     public:
        virtual ~Shape() = default;
        
        /* Returns the material used for rendering, etc. */
        virtual const Material *material() const = 0;
        
        /* Returns the shape / ray intersect (calculates all hit properties). */
        virtual Intersect intersect(const Ray &_ray) const = 0;
        
        /* returns bounds for shape */
        virtual const Bounds &bounds() const = 0;
    };

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_SHAPE_H

