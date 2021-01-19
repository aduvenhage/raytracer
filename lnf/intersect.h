#ifndef LIBS_HEADER_INTERSECT_H
#define LIBS_HEADER_INTERSECT_H

#include "constants.h"
#include "ray.h"
#include "vec3.h"
#include "uv.h"

#include <algorithm>


namespace LNF
{
    class Primitive;

    /*
     Container for intersect attributes and products.
     */
    struct Intersect
    {
        Intersect() 
            :m_axis(axisIdentity()),
             m_pPrimitive(nullptr),
             m_fPositionOnRay(-1),
             m_uTriangleIndex(0),
             m_uTraceDepth(0),
             m_uHitIterationCount(0),
             m_bInside(false)
        {}

        Intersect(const Intersect &) = default;
        Intersect(Intersect &&) = default;
        Intersect(Intersect &) = default;

        Intersect &operator=(const Intersect &) = default;
        Intersect &operator=(Intersect &&) = default;

        operator bool () const {
            return m_fPositionOnRay > 0;
        }

        // fields populated by tracer/caller
        Axis                    m_axis;                 // transform used on ray
        Ray                     m_ray;                  // ray used for intersection (may be transformed from original ray)
        const Primitive         *m_pPrimitive;          // object/shape we intersected with
        
        // key fields that should be populated on primitive hit
        float                   m_fPositionOnRay;       // t0

        // fields required to complete intercept/hit
        Vec                     m_position;             // hit position on surface of shape
        Vec                     m_normal;               // normal on surface of shape
        Uv                      m_uv;                   // texture coordinate on surface of shape
        uint32_t                m_uTriangleIndex;       // specific triangle hit
        uint16_t                m_uTraceDepth;          // number of hits (including reflections, etc.)
        uint16_t                m_uHitIterationCount;   // number of iterations to get to hit (for raymarching, etc.)
        bool                    m_bInside;              // true if ray is inside shape
    };


};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_INTERSECT_H

