#ifndef LIBS_HEADER_PLANE_H
#define LIBS_HEADER_PLANE_H

#include "constants.h"
#include "shape.h"
#include "material.h"
#include "vec3.h"
#include "uv.h"


namespace LNF
{
    /* Plane shape class -- fixed ZX plane with normal [0, 1, 0] and origin [0, 0, 0] */
    class Plane        : public Shape
    {
     public:
        Plane()
        {}
        
        Plane(const Material *_pMaterial, float _fUvScale=0.02f)
            :m_pMaterial(_pMaterial),
             m_fUvScale(_fUvScale)
        {}
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial;
        }
        
        /* Returns the shape / ray intersect (calculates all hit properties). */
        virtual Intersect intersect(const Ray &_ray) const override {
            Intersect ret;
            const static Vec normal(0, 1, 0);
            const float denom = _ray.m_direction.m_fY;
            if (denom < -0.0000001f) {
                const auto vecRayPlane = -_ray.m_origin;
                const float t = vecRayPlane.m_fY / denom;
                if ( (t > _ray.m_fMinDist) && (t < _ray.m_fMaxDist) ) {
                    ret.m_pShape = this;
                    ret.m_fPositionOnRay = t;
                    ret.m_position = _ray.position(ret.m_fPositionOnRay);
                    ret.m_normal = normal;
                    
                    ret.m_uv = Uv(ret.m_position.m_fX * m_fUvScale, ret.m_position.m_fZ * m_fUvScale).wrap();
                }
            }
            
            return ret;
        }
        
     private:
        const Material      *m_pMaterial;
        float               m_fUvScale;
    };


    /* Disc (plane within a certain radius; fixed ZX plane with normal [0, 1, 0] and origin [0, 0, 0]) shape class */
    class Disc        : public Plane
    {
     public:
        Disc()
        {}
        
        Disc(float _fRadius, const Material *_pMaterial, float _fUvScale=0.02f)
            :Plane(_pMaterial, _fUvScale),
             m_bounds(Vec(_fRadius, 1, _fRadius), Vec(-_fRadius, -1, -_fRadius)),
             m_fRadiusSqr(_fRadius * _fRadius)
        {}
        
        /* Returns the shape / ray intersect (calculates all hit properties). */
        virtual Intersect intersect(const Ray &_ray) const override {
            Intersect ret = Plane::intersect(_ray);
            if (ret == true) {
                // check disc bounds
                if (ret.m_position.sizeSqr() > m_fRadiusSqr) {
                    return Intersect();
                }
            }
            
            return ret;
        }
                        
        /* returns bounds for shape */
        virtual const Bounds &bounds() const override {
            return  m_bounds;
        }
        
     private:
        Bounds                      m_bounds;
        float                       m_fRadiusSqr;
    };


    /* Rectangle (plane within a certain width and length; fixed ZX plane with normal [0, 1, 0] and origin [0, 0, 0]) shape class */
    class Rectangle        : public Plane
    {
     public:
        Rectangle()
        {}
        
        Rectangle(float _fWidth, float _fLength, const Material *_pMaterial, float _fUvScale=0.02f)
            :Plane(_pMaterial, _fUvScale),
             m_bounds(Vec(_fWidth, 1, _fLength), Vec(-_fWidth, -1, -_fLength)),
             m_fWidth(_fWidth),
             m_fLength(_fLength)
        {}
        
        /* Returns the shape / ray intersect (calculates all hit properties). */
        virtual Intersect intersect(const Ray &_ray) const override {
            Intersect ret = Plane::intersect(_ray);
            
            if (ret == true) {
                // check rectangle bounds
                if ( (fabs(ret.m_position.m_fX) > m_fWidth) ||
                     (fabs(ret.m_position.m_fZ) > m_fLength) ) {
                    return Intersect();
                }
            }
            
            return ret;
        }
                        
        /* returns bounds for shape */
        virtual const Bounds &bounds() const override {
            return  m_bounds;
        }

     private:
        Bounds                      m_bounds;
        float                       m_fWidth;
        float                       m_fLength;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_PLANE_H

