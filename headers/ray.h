#ifndef LIBS_HEADER_RAY_H
#define LIBS_HEADER_RAY_H

#include "constants.h"
#include "vec3.h"

#define _USE_MATH_DEFINES
#include <cmath>


namespace LNF
{
    struct Ray
    {
        Ray(const Vec &_origin, const Vec &_direction)
            :m_origin(_origin),
             m_direction(_direction)
        {}
        
        Vec position(double _dt) const {
            return m_origin + m_direction * _dt;
        }

        Vec     m_origin;
        Vec     m_direction;
    };


    struct Intersect
    {
        Intersect()
            :m_dPositionOnRay(0)
        {}

        Intersect(const Ray &_ray, double _dPositionOnRay)
            :m_dPositionOnRay(_dPositionOnRay),
             m_position(_ray.position(_dPositionOnRay))
        {}
        
        operator bool () const {
            return m_dPositionOnRay > 0;
        }

        double  m_dPositionOnRay;
        Vec     m_position;
    };


};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_RAY_H

