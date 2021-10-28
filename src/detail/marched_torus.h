#ifndef DETAIL_MARCHED_TORUS_H
#define DETAIL_MARCHED_TORUS_H

#include "core/constants.h"
#include "core/uv.h"
#include "core/vec3.h"
#include "base/primitive.h"
#include "systems/trace.h"
#include "signed_distance_functions.h"


namespace DETAIL
{
    class MarchedTorus        : public BASE::Primitive
    {
     public:
        MarchedTorus(float _fA, float _fB, const BASE::Material *_pMaterial)
            :m_bounds(CORE::boxVec(-100), CORE::boxVec(100)),
             m_pMaterial(_pMaterial),
             m_fA(_fA),
             m_fB(_fB)
        {}
        
        /* Returns the material used for rendering, etc. */
        const BASE::Material *material() const override {
            return m_pMaterial;
        }

        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(BASE::Intersect &_hit) const override {
            auto bi = aaboxIntersect(m_bounds, _hit.m_priRay);
            if (bi.m_intersect == true) {
                // try to hit surface inside (using raymarching)
                bool is_hit = SYSTEMS::check_marched_hit(_hit,
                                                         bi.m_tmax,
                                                         [this](const CORE::Vec &_p){
                                                            return UTILS::sdfTorus(_p, m_fA, m_fB);
                                                         });

                if ( (is_hit == true) &&
                     (_hit.m_priRay.inside(_hit.m_fPositionOnRay) == true) )
                {
                    return true;
                }
            }
            
            return false;
        }

        /* Completes the node intersect properties. */
        virtual BASE::Intersect &intersect(BASE::Intersect &_hit) const override {
            _hit.m_normal = surfaceNormal(_hit.m_position);
            _hit.m_uv = surfaceUv(_hit.m_normal);
            return _hit;
        }
 
        /* returns bounds for shape */
        virtual const CORE::Bounds &bounds() const override {
            return  m_bounds;
        }

     protected:
        // get normal from surface function
        CORE::Vec surfaceNormal(const CORE::Vec &_p) const {
            return CORE::surfaceNormal(_p, [this](const CORE::Vec &_x){
                return UTILS::sdfTorus(_x, m_fA, m_fB);
            });
        }

        // calc surface UV
        CORE::Uv surfaceUv(const CORE::Vec &_n) const {
            return getSphericalUv(_n);
        }

     private:
        CORE::Bounds           m_bounds;
        const BASE::Material   *m_pMaterial;
        float                  m_fA;
        float                  m_fB;
    };

};  // namespace DETAIL


#endif  // #ifndef DETAIL_MARCHED_TORUS_H

