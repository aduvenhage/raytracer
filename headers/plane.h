#ifndef LIBS_HEADER_PLANE_H
#define LIBS_HEADER_PLANE_H

#include "constants.h"
#include "shape.h"
#include "material.h"

#include <memory>


namespace LNF
{
    /* Plane shape class -- fixed ZX plane with normal [0, 1, 0] and origin [0, 0, 0] */
    class Plane        : public Shape
    {
     public:
        Plane()
        {}
        
        Plane(const std::shared_ptr<Material> &_pMaterial, double _dUvScale=0.02)
            :m_pMaterial(_pMaterial),
             m_dUvScale(_dUvScale)
        {}
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial.get();
        }
        
        /* Returns the shape / ray intersect (calculates all hit properties). */
        virtual Intersect intersect(const Ray &_ray) const override {
            Intersect ret;
            const static Vec normal(0, 1, 0);
            const double denom = _ray.m_direction.m_dY;
            if (denom < -0.0000001) {
                const auto vecRayPlane = -_ray.m_origin;
                const double t = vecRayPlane.m_dY / denom;
                if ( (t > _ray.m_dMinDist) && (t < _ray.m_dMaxDist) ) {
                    ret.m_pShape = this;
                    ret.m_dPositionOnRay = t;
                    ret.m_position = _ray.position(ret.m_dPositionOnRay);
                    ret.m_normal = normal;
                    
                    ret.m_uv = Uv(ret.m_position.m_dX * m_dUvScale, ret.m_position.m_dZ * m_dUvScale).wrap();
                }
            }
            
            return ret;
        }
        
     private:
        std::shared_ptr<Material>   m_pMaterial;
        double                      m_dUvScale;
    };


    /* Disc (plane within a certain radius) shape class */
    class Disc        : public Plane
    {
     public:
        Disc()
        {}
        
        Disc(double _dRadius, const std::shared_ptr<Material> &_pMaterial, double _dUvScale=0.02)
            :Plane(_pMaterial, _dUvScale),
             m_dRadius(_dRadius),
             m_dRadiusSqr(_dRadius * _dRadius)
        {}
        
        /* Returns the shape / ray intersect (calculates all hit properties). */
        virtual Intersect intersect(const Ray &_ray) const override {
            Intersect ret = Plane::intersect(_ray);
            if (ret == true) {
                // check disc bounds
                if (ret.m_position.sizeSqr() > m_dRadiusSqr) {
                    return Intersect();
                }
            }
            
            return ret;
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
        
        Rectangle(double _dWidth, double _dLength, const std::shared_ptr<Material> &_pMaterial, double _dUvScale=0.02)
            :Plane(_pMaterial, _dUvScale),
             m_dWidth(_dWidth),
             m_dLength(_dLength)
        {}
        
        /* Returns the shape / ray intersect (calculates all hit properties). */
        virtual Intersect intersect(const Ray &_ray) const override {
            Intersect ret = Plane::intersect(_ray);
            
            if (ret == true) {
                // check rectangle bounds
                if ( (fabs(ret.m_position.m_dX) > m_dWidth) ||
                     (fabs(ret.m_position.m_dZ) > m_dLength) ) {
                    return Intersect();
                }
            }
            
            return ret;
        }
        
     private:
        double                      m_dWidth;
        double                      m_dLength;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_PLANE_H

