#ifndef LIBS_HEADER_PLANE_H
#define LIBS_HEADER_PLANE_H

#include "constants.h"
#include "shape.h"
#include "material.h"

#include <memory>


namespace LNF
{
    /* Plane shape class */
    class Plane        : public Shape
    {
     public:
        Plane()
        {}
        
        Plane(const Vec &_origin, const Vec &_normal, std::unique_ptr<Material> _pMaterial, double _dUvScale=0.02)
            :m_origin(_origin),
             m_plane(axisPlane(_normal, _origin)),
             m_pMaterial(std::move(_pMaterial)),
             m_dUvScale(_dUvScale)
        {}
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const {
            return m_pMaterial.get();
        }
        
        /*
         Returns the point (t) on the ray where it intersects this shape.
         Returns 0.0 if there is no intersect possible.
         */
        virtual double intersect(const Ray &_ray) const {
            const double denom = m_plane.m_y * _ray.m_direction;
            if (denom < -0.0000001) {
                const auto vecRayPlane = m_origin - _ray.m_origin;
                const double t = (vecRayPlane * m_plane.m_y) / denom;
                if ( (t > _ray.m_dMinDist) && (t < _ray.m_dMaxDist) ) {
                    return t;
                }
            }
            
            return 0.0;
        }
        
        /* Returns the shape normal vector at the given surface position. */
        virtual Vec normal(const Vec &_pos) const {
            return m_plane.m_y;
        }
        
        /* Returns the plane origin */
        const Vec &origin() const {return m_origin;}

        /* Returns the plane axis */
        const Axis &axis() const {return m_plane;}

        /* Returns texture coordinates on given surface position. */
        virtual Uv uv(const Vec &_pos) const {
            const auto vec = _pos - m_origin;
            return Uv(m_plane.m_x * vec * m_dUvScale, m_plane.m_z * vec * m_dUvScale).wrap();
        }
        
     private:
        Vec                         m_origin;
        Axis                        m_plane;        // [x=e1, y=normal, z=e2]
        std::unique_ptr<Material>   m_pMaterial;
        double                      m_dUvScale;
    };


    /* Disc (plane within a certain radius) shape class */
    class Disc        : public Plane
    {
     public:
        Disc()
        {}
        
        Disc(const Vec &_origin, const Vec &_normal, double _dRadius, std::unique_ptr<Material> _pMaterial, double _dUvScale=0.02)
            :Plane(_origin, _normal, std::move(_pMaterial), _dUvScale),
             m_dRadius(_dRadius),
             m_dRadiusSqr(_dRadius * _dRadius)
        {}
        
        /*
         Returns the point (t) on the ray where it intersects this shape.
         Returns 0.0 if there is no intersect possible.
         */
        virtual double intersect(const Ray &_ray) const {
            const double t = Plane::intersect(_ray);
            if (t > 0) {
                auto p = _ray.position(t) - origin();
                if (p.sizeSqr() < m_dRadiusSqr) {
                    return t;
                }
            }
            
            return 0.0;
        }
        
     private:
        double                      m_dRadius;
        double                      m_dRadiusSqr;
    };


    /* Rectangle (plane within a certain width and length) shape class */
    class Rectangle        : public Plane
    {
     public:
        Rectangle()
        {}
        
        Rectangle(const Vec &_origin, const Vec &_normal, double _dWidth, double _dLength, std::unique_ptr<Material> _pMaterial, double _dUvScale=0.02)
            :Plane(_origin, _normal, std::move(_pMaterial), _dUvScale),
             m_dWidth(_dWidth),
             m_dLength(_dLength)
        {}
        
        /*
         Returns the point (t) on the ray where it intersects this shape.
         Returns 0.0 if there is no intersect possible.
         */
        virtual double intersect(const Ray &_ray) const {
            const double t = Plane::intersect(_ray);
            if (t > 0) {
                auto p = _ray.position(t) - origin();
                
                if ( (fabs(p * axis().m_x) <= m_dWidth) &&
                     (fabs(p * axis().m_z) <= m_dLength) ) {
                    return t;
                }
            }
            
            return 0.0;
        }
        
     private:
        double                      m_dWidth;
        double                      m_dLength;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_PLANE_H

