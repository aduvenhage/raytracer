#ifndef LIBS_HEADER_SHAPE_H
#define LIBS_HEADER_SHAPE_H

#include "constants.h"
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
        
        /* Returns the point (t) on the ray where it intersects this shape. */
        virtual double intersect(const Ray &_ray) const = 0;
        
        /* Returns the shape normal vector at the given surface position. */
        virtual Vec normal(const Vec &_pos) const = 0;
        
        /* Returns texture coordinates on given surface position. */
        virtual Uv uv(const Vec &_pos) const = 0;
            
        /* Returns the diffuse color at the given surface position */
        virtual Color color(const Uv &_uv) const = 0;
        
        /* Returns material property [0..1] = [diffuse .. mirror] */
        virtual double reflection() = 0;
        
        /* Returns material property [0..1] = [opaque .. clear] */
        virtual double transparancy() = 0;
        
        /* Returns material property */
        virtual double indexOfRefraction() = 0;
    };

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_SHAPE_H

