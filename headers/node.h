#ifndef LIBS_HEADER_NODE_H
#define LIBS_HEADER_NODE_H

#include "constants.h"
#include "intersect.h"
#include "material.h"
#include "ray.h"

#include <memory>


namespace LNF
{
    /*
        Scene Node
        - hit: populates at least node and time properties of intercept for getting quick node hits.
        - intersect: complete intersect properties
        
     */
    class Node
    {
     public:
        virtual ~Node() = default;
        
        /* Returns the material used for rendering, etc. */
        virtual const Material *material() const = 0;
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual Intersect hit(const Ray &_ray) const = 0;
        
        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const = 0;
        
        /* returns bounds for shape */
        virtual const Bounds &bounds() const = 0;
    };
    

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_NODE_H

