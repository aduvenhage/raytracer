#ifndef LIBS_HEADER_PLANE_H
#define LIBS_HEADER_PLANE_H

#include "constants.h"
#include "node.h"
#include "material.h"
#include "vec3.h"
#include "uv.h"


namespace LNF
{
    /* Plane shape class -- fixed ZX plane with normal [0, 1, 0] and origin [0, 0, 0] */
    class Plane        : public Node
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
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual Intersect hit(const Ray &_ray) const override {
            Intersect ret;
            const static Vec normal(0, 1, 0);
            const float denom = _ray.m_direction.m_fY;
            if (denom < -0.0000001f) {
                const auto vecRayPlane = -_ray.m_origin;
                const float t = vecRayPlane.m_fY / denom;
                if ( (t > _ray.m_fMinDist) && (t < _ray.m_fMaxDist) ) {
                    ret.m_pNode = this;
                    ret.m_fPositionOnRay = t;
                    ret.m_ray = _ray;
                }
            }
            
            return ret;
        }
        
        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            _hit.m_position = _hit.m_ray.position(_hit.m_fPositionOnRay);
            _hit.m_normal = Vec(0, 1, 0);
            _hit.m_uv = Uv(_hit.m_position.m_fX * m_fUvScale, _hit.m_position.m_fZ * m_fUvScale).wrap();
            
            return _hit;
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
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual Intersect hit(const Ray &_ray) const override {
            Intersect ret = Plane::hit(_ray);
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
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual Intersect hit(const Ray &_ray) const override {
            Intersect ret = Plane::hit(_ray);
            
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

