#ifndef LIBS_HEADER_SPHERE_H
#define LIBS_HEADER_SPHERE_H

#include "color.h"
#include "constants.h"
#include "ray.h"
#include "uv.h"
#include "vec3.h"

#define _USE_MATH_DEFINES
#include <cmath>


namespace LNF
{
    struct Sphere
    {
        Sphere()
            :m_dRadius(0),
             m_dRadiusSqr(0)
        {}
        
        Sphere(const Vec &_origin, double _dRadius, const Color &_color)
            :m_origin(_origin),
             m_color(_color),
             m_dRadius(0),
             m_dRadiusSqr(_dRadius * _dRadius)
        {}
        
        double intersect(const Ray &_ray) const {
            auto vecRaySphere = m_origin - _ray.m_origin;
            double dRayLength = vecRaySphere * _ray.m_direction;
            if (dRayLength <= 0) return Intersect();
            
            double dIntersectRadiusSqr = vecRaySphere.sizeSqr() - dRayLength*dRayLength;
            if (dIntersectRadiusSqr > m_dRadiusSqr) return Intersect();
            
            return dRayLength - sqrt(m_dRadiusSqr - dIntersectRadiusSqr);
        }
        
        Uv uv(const Vec &_pos) const {
            auto vec = _pos - m_origin;
            return Uv(vec.heading() / M_PI / 2 + 0.5,
                      vec.pitch() / M_PI + 0.5);
        }
        
        Vec normal(const Vec &_pos) const {
            return (_pos - m_origin).normalize();
        }

        Vec         m_origin;
        Color       m_color;
        double      m_dRadius;
        double      m_dRadiusSqr;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_SPHERE_H

