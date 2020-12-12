#ifndef LIBS_HEADER_INTERSECT_H
#define LIBS_HEADER_INTERSECT_H

#include "constants.h"
#include "ray.h"
#include "vec3.h"
#include "uv.h"

#include <algorithm>


namespace LNF
{
    class Node;

    /*
     Container for intersect attributes and products.
     */
    struct Intersect
    {
        Intersect() 
            :m_pNode(nullptr),
             m_axis(axisIdentity()),
             m_fPositionOnRay(-1),
             m_uTriangleIndex(0),
             m_uIterationCount(0),
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

        const Node              *m_pNode;           // node we intersected with
        Axis                    m_axis;             // transform used on ray
        Ray                     m_ray;              // ray used for intersection (may be transformed from original ray)
        Vec                     m_position;         // hit position on surface of shape
        Vec                     m_normal;           // normal on surface of shape
        Uv                      m_uv;               // texture coordinate on surface of shape
        float                   m_fPositionOnRay;   // t0
        uint32_t                m_uTriangleIndex;   // specific triangle hit on node
        uint16_t                m_uIterationCount;  // number of ray iterations before hit
        bool                    m_bInside;          // true if ray is inside shape
    };


};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_INTERSECT_H

