#ifndef LIBS_HEADER_INTERSECT_H
#define LIBS_HEADER_INTERSECT_H

#include "constants.h"
#include "ray.h"
#include "vec3.h"
#include "uv.h"

#include <algorithm>


namespace LNF
{
    class Shape;

    /*
     Container for intersect attributes and products.
     */
    struct Intersect
    {
        Intersect()
            :m_pShape(nullptr),
             m_dPositionOnRay(-1),
             m_bInside(false)
        {}

        Intersect(const Intersect &) = default;
        Intersect(Intersect &&) = default;
        Intersect(Intersect &) = default;

        Intersect &operator=(const Intersect &) = default;
        Intersect &operator=(Intersect &&) = default;

        operator bool () const {
            return m_dPositionOnRay > 0;
        }

        bool operator < (const Intersect &_i) const {
            if ( (_i.m_dPositionOnRay > 0) &&
                 (m_dPositionOnRay > 0) ) {
                return m_dPositionOnRay < _i.m_dPositionOnRay;
            }
            
            return m_dPositionOnRay > 0;
        }

        const Shape             *m_pShape;          // shape we intersected with
        float                   m_dPositionOnRay;   // t0
        Vec                     m_position;         // hit position on surface of shape
        Vec                     m_normal;           // normal on surface of shape
        Uv                      m_uv;               // texture coordinate on surface of shape
        bool                    m_bInside;          // true if ray is inside shape
    };


};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_INTERSECT_H

