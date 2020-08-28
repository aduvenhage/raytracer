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
             m_normal(_normal),
             m_pMaterial(std::move(_pMaterial)),
             m_dUvScale(_dUvScale)
        {
            m_e1 = crossProduct(m_normal, Vec(0.0, 0.0, 1.0));
            if (m_e1.sizeSqr() < 0.0001) {
                m_e1 = crossProduct(m_normal, Vec(0.0, 1.0, 0.0));
            }
            
            m_e2 = crossProduct(m_normal, m_e1);
        }
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const {
            return m_pMaterial.get();
        }
        
        /*
         Returns the point (t) on the ray where it intersects this shape.
         Returns 0.0 if there is no intersect possible.
         */
        virtual double intersect(const Ray &_ray) const {
            const double denom = m_normal * _ray.m_direction;
            if (denom < -0.0000001) {
                const auto vecRayPlane = m_origin - _ray.m_origin;
                const double t = (vecRayPlane * m_normal) / denom;
                if ( (t > _ray.m_dMinDist) && (t < _ray.m_dMaxDist) ) {
                    return t;
                }
            }
            
            return 0.0;
        }
        
        /* Returns the shape normal vector at the given surface position. */
        virtual Vec normal(const Vec &_pos) const {
            return m_normal;
        }

        /* Returns texture coordinates on given surface position. */
        virtual Uv uv(const Vec &_pos) const {
            const auto vec = _pos - m_origin;
            return Uv(m_e1 * vec * m_dUvScale, m_e2 * vec * m_dUvScale).wrap();
        }
        
     private:
        Vec                         m_origin;
        Vec                         m_normal;
        Vec                         m_e1;
        Vec                         m_e2;
        std::unique_ptr<Material>   m_pMaterial;
        double                      m_dUvScale;
    };


    /* Disc (plane within a certain radius) shape class */
    class Disc        : public Shape
    {
     public:
        Disc()
        {}
        
        Disc(const Vec &_origin, const Vec &_normal, double _dRadius, std::unique_ptr<Material> _pMaterial, double _dUvScale=0.02)
            :m_origin(_origin),
             m_normal(_normal),
             m_pMaterial(std::move(_pMaterial)),
             m_dRadius(_dRadius),
             m_dRadiusSqr(_dRadius * _dRadius),
             m_dUvScale(_dUvScale)
        {
            m_e1 = crossProduct(m_normal, Vec(0.0, 0.0, 1.0));
            if (m_e1.sizeSqr() < 0.0001) {
                m_e1 = crossProduct(m_normal, Vec(0.0, 1.0, 0.0));
            }
            
            m_e2 = crossProduct(m_normal, m_e1);
        }
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const {
            return m_pMaterial.get();
        }
        
        /*
         Returns the point (t) on the ray where it intersects this shape.
         Returns 0.0 if there is no intersect possible.
         */
        virtual double intersect(const Ray &_ray) const {
            const double denom = m_normal * _ray.m_direction;
            if (denom < -0.0000001) {
                const auto vecRayPlane = m_origin - _ray.m_origin;
                const double t = (vecRayPlane * m_normal) / denom;
                if ( (t > _ray.m_dMinDist) && (t < _ray.m_dMaxDist) ) {
                    auto p = _ray.position(t) - m_origin;
                    if (p.sizeSqr() < m_dRadiusSqr) {
                        return t;
                    }
                }
            }
            
            return 0.0;
        }
        
        /* Returns the shape normal vector at the given surface position. */
        virtual Vec normal(const Vec &_pos) const {
            return m_normal;
        }

        /* Returns texture coordinates on given surface position. */
        virtual Uv uv(const Vec &_pos) const {
            const auto p = _pos - m_origin;
            return Uv(m_e1 * p * m_dUvScale, m_e2 * p * m_dUvScale).wrap();
        }
        
     private:
        Vec                         m_origin;
        Vec                         m_normal;
        Vec                         m_e1;
        Vec                         m_e2;
        std::unique_ptr<Material>   m_pMaterial;
        double                      m_dRadius;
        double                      m_dRadiusSqr;
        double                      m_dUvScale;
    };


    /* Rectangle (plane within a certain width and length) shape class */
    class Rectangle        : public Shape
    {
     public:
        Rectangle()
        {}
        
        Rectangle(const Vec &_origin, const Vec &_normal, double _dWidth, double _dLength, std::unique_ptr<Material> _pMaterial, double _dUvScale=0.02)
            :m_origin(_origin),
             m_normal(_normal),
             m_pMaterial(std::move(_pMaterial)),
             m_dWidth(_dWidth),
             m_dLength(_dLength),
             m_dUvScale(_dUvScale)
        {
            m_e1 = crossProduct(m_normal, Vec(0.0, 0.0, 1.0));
            if (m_e1.sizeSqr() < 0.0001) {
                m_e1 = crossProduct(m_normal, Vec(0.0, 1.0, 0.0));
            }
            
            m_e2 = crossProduct(m_normal, m_e1);
        }
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const {
            return m_pMaterial.get();
        }
        
        /*
         Returns the point (t) on the ray where it intersects this shape.
         Returns 0.0 if there is no intersect possible.
         */
        virtual double intersect(const Ray &_ray) const {
            const double denom = m_normal * _ray.m_direction;
            if (denom < -0.0000001) {
                const auto vecRayPlane = m_origin - _ray.m_origin;
                const double t = (vecRayPlane * m_normal) / denom;
                if ( (t > _ray.m_dMinDist) && (t < _ray.m_dMaxDist) ) {
                    auto p = _ray.position(t) - m_origin;
                    
                    if ( (fabs(p * m_e1) <= m_dWidth) &&
                         (fabs(p * m_e2) <= m_dLength) ) {
                        return t;
                    }
                }
            }
            
            return 0.0;
        }
        
        /* Returns the shape normal vector at the given surface position. */
        virtual Vec normal(const Vec &_pos) const {
            return m_normal;
        }

        /* Returns texture coordinates on given surface position. */
        virtual Uv uv(const Vec &_pos) const {
            const auto p = _pos - m_origin;
            return Uv(m_e1 * p * m_dUvScale, m_e2 * p * m_dUvScale).wrap();
        }
        
     private:
        Vec                         m_origin;
        Vec                         m_normal;
        Vec                         m_e1;
        Vec                         m_e2;
        std::unique_ptr<Material>   m_pMaterial;
        double                      m_dWidth;
        double                      m_dLength;
        double                      m_dUvScale;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_PLANE_H

