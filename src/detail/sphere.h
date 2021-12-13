#ifndef DETAIL_SPHERE_H
#define DETAIL_SPHERE_H

#include "core/constants.h"
#include "core/vec3.h"
#include "core/uv.h"
#include "base/primitive.h"


namespace DETAIL
{
    /* Sphere shape class -- fixed at origin [0, 0, 0] */
    class Sphere        : public BASE::Primitive
    {
     public:
        Sphere()
            :m_fRadius(0),
             m_fRadiusSqr(0),
             m_bOnlyHitOutside(false)
        {}
        
        Sphere(float _fRadius, const BASE::Material *_pMaterial, bool _bOnlyHitOutside = false)
            :m_pMaterial(_pMaterial),
             m_bounds(CORE::Vec(-_fRadius, -_fRadius, -_fRadius), CORE::Vec(_fRadius, _fRadius, _fRadius)),
             m_fRadius(_fRadius),
             m_fRadiusSqr(_fRadius * _fRadius),
             m_bOnlyHitOutside(_bOnlyHitOutside)
        {}
        
        /* Returns the material used for rendering, etc. */
        const BASE::Material *material() const override {
            return m_pMaterial;
        }
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(BASE::Intersect &_hit) const override {
            float dRayLength = -_hit.m_priRay.m_origin * _hit.m_priRay.m_direction.normalized();
            if (const float dIntersectRadiusSqr = _hit.m_priRay.m_origin.sizeSqr() - dRayLength*dRayLength; dIntersectRadiusSqr <= m_fRadiusSqr) {
                const float dt = sqrt(m_fRadiusSqr - dIntersectRadiusSqr);
                if (const float t0 = dRayLength - dt; _hit.m_priRay.inside(t0) == true) {
                    // we are outside of sphere
                    _hit.m_bInside = false;
                    _hit.m_fPositionOnRay = t0;
                    return true;
                }

                if (m_bOnlyHitOutside == false) {
                    if (const float t1 = dRayLength + dt; _hit.m_priRay.inside(t1) == true) {
                        // we are inside of sphere
                        _hit.m_bInside = true;
                        _hit.m_fPositionOnRay = t1;
                        return true;
                    }
                }
            }

            return false;
        }

        /* Completes the node intersect properties. */
        virtual BASE::Intersect &intersect(BASE::Intersect &_hit) const override {
            _hit.m_position = _hit.m_priRay.position(_hit.m_fPositionOnRay);
            _hit.m_normal = _hit.m_position / m_fRadius;
            _hit.m_uv = getSphericalUv(_hit.m_normal);

            return _hit;
        }

        /* returns bounds for shape */
        virtual const CORE::Bounds &bounds() const override {
            return  m_bounds;
        }

     private:
        const BASE::Material    *m_pMaterial;
        CORE::Bounds            m_bounds;
        float                   m_fRadius;
        float                   m_fRadiusSqr;
        bool                    m_bOnlyHitOutside;
    };

};  // namespace DETAIL

#endif  // #ifndef DETAIL_SPHERE_H

