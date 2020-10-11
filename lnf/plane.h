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
        virtual bool hit(Intersect &_hit, const Ray &_ray) const override {
            const float denom = _ray.m_direction.y();
            if (denom < -0.0000001f) {
                const auto vecRayPlane = -_ray.m_origin;
                const float t = vecRayPlane.y() / denom;
                if ( (t > _ray.m_fMinDist) && (t < _ray.m_fMaxDist) ) {
                    _hit.m_pNode = this;
                    _hit.m_fPositionOnRay = t;
                    _hit.m_ray = _ray;

                    return true;
                }
            }
            
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            _hit.m_position = _hit.m_ray.position(_hit.m_fPositionOnRay);
            _hit.m_normal = Vec(0, 1, 0);
            _hit.m_uv = Uv(_hit.m_position.x() * m_fUvScale, _hit.m_position.z() * m_fUvScale).wrap();
            
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
        virtual bool hit(Intersect &_hit, const Ray &_ray) const override {
            if (Plane::hit(_hit, _ray) == true) {
                // check disc bounds
                return _hit.m_position.sizeSqr() < m_fRadiusSqr;
            }
            
            return false;
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
        virtual bool hit(Intersect &_hit, const Ray &_ray) const override {
            if (Plane::hit(_hit, _ray) == true) {
                // check rectangle bounds
                return (fabs(_hit.m_position.x()) <= m_fWidth) &&
                       (fabs(_hit.m_position.z()) <= m_fLength);
            }
            
            return false;
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

