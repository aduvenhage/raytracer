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

    struct Intersect
    {
        Intersect()
            :m_dPositionOnRay(0)
        {}

        Intersect(const Vec &_position, double _dPositionOnRay)
            :m_dPositionOnRay(_dPositionOnRay),
             m_position(_position)
        {}

        Intersect(const Shape *_pShape, const Ray &_ray, double _dPositionOnRay)
            :m_pShape(_pShape),
             m_dPositionOnRay(_dPositionOnRay),
             m_position(_ray.position(_dPositionOnRay))
        {}

        operator bool () const {
            return m_dPositionOnRay > 0;
        }

        const Shape             *m_pShape;          // shape we intersected with
        double                  m_dPositionOnRay;   // t0
        Vec                     m_position;         // position on surface of shape
        Vec                     m_normal;           // normal vector on surface of shape
        Uv                      m_uv;               // texture coordinate on surface of shape
    };


};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_INTERSECT_H
