#ifndef DETAIL_PLANE_H
#define DETAIL_PLANE_H

#include "core/constants.h"
#include "core/vec3.h"
#include "core/uv.h"
#include "base/primitive.h"
#include "base/material.h"


namespace DETAIL
{
    /* Plane shape class -- fixed ZX plane with normal [0, 1, 0] and origin [0, 0, 0] */
    class Plane        : public BASE::Primitive
    {
     public:
        Plane()
        {}
        
        Plane(const BASE::Material *_pMaterial, float _fUvScale=0.02f)
            :m_pMaterial(_pMaterial),
             m_fUvScale(_fUvScale)
        {}
        
        /* Returns the material used for rendering, etc. */
        const BASE::Material *material() const override {
            return m_pMaterial;
        }
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(BASE::Intersect &_hit) const override {
            // NOTE: only hit if ray is pointing down at plane
            const float denom = _hit.m_priRay.m_direction.y();
            if (denom < -0.00001f) {
                const auto vecRayPlane = -_hit.m_priRay.m_origin;
                const float t = vecRayPlane.y() / denom;
                if (_hit.m_priRay.inside(t) == true) {
                    _hit.m_fPositionOnRay = t;
                    return true;
                }
            }
            
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual BASE::Intersect &intersect(BASE::Intersect &_hit) const override {
            _hit.m_position = _hit.m_priRay.position(_hit.m_fPositionOnRay);
            _hit.m_normal = CORE::Vec(0, 1, 0);
            _hit.m_uv = uv(_hit.m_position);
            
            return _hit;
        }
        
        virtual CORE::Uv uv(const CORE::Vec &_p) const {
            return CORE::Uv(_p.x() * m_fUvScale, _p.z() * m_fUvScale).wrap();
        }
        
     private:
        const BASE::Material    *m_pMaterial;
        float                   m_fUvScale;
    };


    /* Disc (plane within a certain radius; fixed ZX plane with normal [0, 1, 0] and origin [0, 0, 0]) shape class */
    class Disc        : public Plane
    {
     public:
        Disc()
        {}
        
        Disc(float _fRadius, const BASE::Material *_pMaterial, float _fUvScale=0.02f)
            :Plane(_pMaterial, _fUvScale),
             m_bounds(CORE::Vec(-_fRadius, -0.5, -_fRadius), CORE::Vec(_fRadius, 0.5, _fRadius)),
             m_fRadiusSqr(_fRadius * _fRadius)
        {}
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(BASE::Intersect &_hit) const override {
            if (Plane::hit(_hit) == true) {
                // check disc bounds
                _hit.m_position = _hit.m_priRay.position(_hit.m_fPositionOnRay);
                return _hit.m_position.sizeSqr() < m_fRadiusSqr;
            }
            
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual BASE::Intersect &intersect(BASE::Intersect &_hit) const override {
            _hit.m_normal = CORE::Vec(0, 1, 0);
            _hit.m_uv = uv(_hit.m_position);

            return _hit;
        }
        
        /* returns bounds for shape */
        virtual const CORE::Bounds &bounds() const override {
            return  m_bounds;
        }
        
     private:
        CORE::Bounds                m_bounds;
        float                       m_fRadiusSqr;
    };


    /*
     Rectangle (plane within a certain width and length; fixed ZX plane with normal [0, 1, 0] and origin [0, 0, 0]) shape class
     */
    class Rectangle        : public Plane
    {
     public:
        Rectangle()
        {}
        
        Rectangle(float _fWidth, float _fLength, const BASE::Material *_pMaterial, float _fUvScale=0.02f)
            :Plane(_pMaterial, _fUvScale),
             m_bounds(CORE::Vec(-_fWidth/2, -0.5, -_fLength/2), CORE::Vec(_fWidth/2, 0.5, _fLength/2)),
             m_fWidth(_fWidth),
             m_fLength(_fLength)
        {}
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(BASE::Intersect &_hit) const override {
            if (Plane::hit(_hit) == true) {
                // check rectangle bounds
                _hit.m_position = _hit.m_priRay.position(_hit.m_fPositionOnRay);
                return (fabs(_hit.m_position.x()) <= m_fWidth) &&
                       (fabs(_hit.m_position.z()) <= m_fLength);
            }
            
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual BASE::Intersect &intersect(BASE::Intersect &_hit) const override {
            _hit.m_normal = CORE::Vec(0, 1, 0);
            _hit.m_uv = uv(_hit.m_position);

            return _hit;
        }
        
        /* returns bounds for shape */
        virtual const CORE::Bounds &bounds() const override {
            return  m_bounds;
        }

     private:
        CORE::Bounds                m_bounds;
        float                       m_fWidth;
        float                       m_fLength;
    };


};  // namespace DETAIL

#endif  // #ifndef DETAIL_PLANE_H

