#ifndef LIBS_HEADER_RAY_H
#define LIBS_HEADER_RAY_H

#include "constants.h"
#include "vec3.h"


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


    inline Vec reflect(const Vec &_vec, const Vec _normal) {
        // refl = ray - nhit * 2 * (ray . nhit)
        return _vec - _normal * 2 * (_vec * _normal);
    }


};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_RAY_H

