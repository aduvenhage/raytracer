#ifndef LIBS_HEADER_SHAPE_H
#define LIBS_HEADER_SHAPE_H

#include "constants.h"
#include "material.h"
#include "ray.h"
#include "uv.h"
#include "vec3.h"

#include <memory>


namespace LNF
{
    /* Base class for all Shapes */
    class Shape
    {
     public:
        virtual ~Shape() = default;
        
        /* Returns the material used for rendering, etc. */
        virtual const Material *material() const = 0;
        
        /* Returns the point (t) on the ray where it intersects this shape. */
        virtual double intersect(const Ray &_ray, double _dMin, double _dMax) const = 0;
        
        /* Returns the shape normal vector at the given surface position. */
        virtual Vec normal(const Vec &_pos) const = 0;
        
        /* Returns texture coordinates on given surface position. */
        virtual Uv uv(const Vec &_pos) const = 0;
    };

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_SHAPE_H

