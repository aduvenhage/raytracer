#ifndef LIBS_HEADER_PLANE_H
#define LIBS_HEADER_PLANE_H

#include "constants.h"
#include "primitive.h"
#include "material.h"
#include "vec3.h"
#include "uv.h"


namespace LNF
{
    /* Plane shape class -- fixed ZX plane with normal [0, 1, 0] and origin [0, 0, 0] */
    class Plane        : public Primitive
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
        virtual bool hit(Intersect &_hit, RandomGen &) const override {
            const float denom = _hit.m_ray.m_direction.y();
            if (denom < -0.0000001f) {
                const auto vecRayPlane = -_hit.m_ray.m_origin;
                const float t = vecRayPlane.y() / denom;
                if ( (t > _hit.m_ray.m_fMinDist) && (t < _hit.m_ray.m_fMaxDist) ) {
                    _hit.m_fPositionOnRay = t;
                    return true;
                }
            }
            
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            _hit.m_position = _hit.m_ray.position(_hit.m_fPositionOnRay);
            _hit.m_normal = Vec(0, 1, 0);
            _hit.m_uv = uv(_hit.m_position);
            
            return _hit;
        }
        
        virtual Uv uv(const Vec &_p) const {
            return Uv(_p.x() * m_fUvScale, _p.z() * m_fUvScale).wrap();
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
             m_bounds(Vec(-_fRadius, -1, -_fRadius), Vec(_fRadius, 1, _fRadius)),
             m_fRadiusSqr(_fRadius * _fRadius)
        {}
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit, RandomGen &_randomGen) const override {
            if (Plane::hit(_hit, _randomGen) == true) {
                // check disc bounds
                _hit.m_position = _hit.m_ray.position(_hit.m_fPositionOnRay);
                return _hit.m_position.sizeSqr() < m_fRadiusSqr;
            }
            
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            _hit.m_normal = Vec(0, 1, 0);
            _hit.m_uv = uv(_hit.m_position);

            return _hit;
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
             m_bounds(Vec(-_fWidth, -1, -_fLength), Vec(_fWidth, 1, _fLength)),
             m_fWidth(_fWidth),
             m_fLength(_fLength)
        {}
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit, RandomGen &_randomGen) const override {
            if (Plane::hit(_hit, _randomGen) == true) {
                // check rectangle bounds
                _hit.m_position = _hit.m_ray.position(_hit.m_fPositionOnRay);
                return (fabs(_hit.m_position.x()) <= m_fWidth) &&
                       (fabs(_hit.m_position.z()) <= m_fLength);
            }
            
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            _hit.m_normal = Vec(0, 1, 0);
            _hit.m_uv = uv(_hit.m_position);

            return _hit;
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

