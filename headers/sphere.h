#ifndef LIBS_HEADER_SPHERE_H
#define LIBS_HEADER_SPHERE_H

#include "color.h"
#include "constants.h"
#include "ray.h"
#include "uv.h"
#include "vec3.h"


namespace LNF
{
    /* Base class for all Shapes */
    class Shape
    {
     public:
        /* Returns the point (t) on the ray where it intersects this shape. */
        virtual double intersect(const Ray &_ray) const = 0;
        
        /* Returns the shape UV coordinate at the given surface position. */
        virtual Uv uv(const Vec &_pos) const = 0;
        
        /* Returns the shape/material color at the given UV position. */
        virtual Color color(const Vec &_pos) const = 0;
        
        /* Returns the shape normal vector at the given surface position. */
        virtual Vec normal(const Vec &_pos) const = 0;
    };


    /* Spehere class */
    class Sphere
    {
     public:
        Sphere()
            :m_dRadius(0),
             m_dRadiusSqr(0),
             m_dTransperancy(0),
             m_dReflection(0)
        {}
        
        Sphere(const Vec &_origin, double _dRadius,
               const Color &_color, double _dTransperancy, double _dReflection)
            :m_origin(_origin),
             m_color(_color),
             m_dRadius(_dRadius),
             m_dRadiusSqr(_dRadius * _dRadius),
             m_dTransperancy(_dTransperancy),
             m_dReflection(_dReflection)
        {}
        
        /* Returns the point (t) on the ray where it intersects this shape. */
        double intersect(const Ray &_ray) const {
            auto vecRaySphere = m_origin - _ray.m_origin;
            double dRayLength = vecRaySphere * _ray.m_direction;
            if (dRayLength <= 0) return Intersect();
            
            double dIntersectRadiusSqr = vecRaySphere.sizeSqr() - dRayLength*dRayLength;
            if (dIntersectRadiusSqr > m_dRadiusSqr) return Intersect();
            
            return dRayLength - sqrt(m_dRadiusSqr - dIntersectRadiusSqr);
        }
        
        /* Returns the shape UV coordinate at the given surface position. */
        Uv uv(const Vec &_pos) const {
            auto vec = _pos - m_origin;
            double phi = atan2(vec.m_dZ, vec.m_dX);
            double theta = acos(vec.m_dY / m_dRadius);

            return Uv(phi / M_PI / 2 + 0.5, theta / M_PI + 0.5);
        }
        
        /* Returns the shape color at the given UV position. */
        Color color(const Vec &_pos) const {
            return m_color;
        }
        
        /* Returns the shape normal vector at the given surface position. */
        Vec normal(const Vec &_pos) const {
            return (_pos - m_origin).normalize();
        }

        Vec         m_origin;
        Color       m_color;
        double      m_dRadius;
        double      m_dRadiusSqr;
        double      m_dTransperancy;
        double      m_dReflection;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_SPHERE_H

