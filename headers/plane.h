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
            const static Vec e1(1, 0, 0);
            const static Vec e2(0, 0, 1);

            // TODO: optimise for fixed axis (above)
            const double denom = normal * _ray.m_direction;
            if (denom < -0.0000001) {
                const auto vecRayPlane = -_ray.m_origin;
                const double t = (vecRayPlane * normal) / denom;
                if ( (t > _ray.m_dMinDist) && (t < _ray.m_dMaxDist) ) {
                    ret.m_pShape = this;
                    ret.m_dPositionOnRay = t;
                    ret.m_position = _ray.position(ret.m_dPositionOnRay);
                    ret.m_normal = normal;
                    
                    ret.m_uv = Uv(e1 * ret.m_position * m_dUvScale, e2 * ret.m_position * m_dUvScale).wrap();
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

