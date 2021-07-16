#ifndef DETAIL_MARCHED_MANDLE_H
#define DETAIL_MARCHED_MANDLE_H

#include "core/uv.h"
#include "core/vec3.h"
#include "core/ray.h"
#include "core/constants.h"
#include "base/primitive.h"
#include "systems/trace.h"
#include "utils/signed_distance_functions.h"


namespace DETAIL
{
    /* Raymarched mandlebulb -- fixed size (2.5 diameter) */
    class MarchedMandle        : public BASE::Primitive
    {
     public:
        MarchedMandle(const BASE::Material *_pMaterial)
            :m_bounds(CORE::boxVec(-1.25), CORE::boxVec(1.25)),
             m_pMaterial(_pMaterial)
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
                int bulbIterations = 0;
                bool is_hit = SYSTEMS::check_marched_hit(_hit,
                                                         bi.m_tmax,
                                                         [&](const CORE::Vec &_p){
                                                            return UTILS::sdfMandle(_p, bulbIterations);
                                                         });
                
                // override iteration count
                _hit.m_uIterations = (uint16_t)bulbIterations;

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
            _hit.m_uv = surfaceUv(_hit.m_position);
            return _hit;
        }
 
        /* returns bounds for shape */
        virtual const CORE::Bounds &bounds() const override {
            return  m_bounds;
        }

     protected:
        // get normal from surface function
        CORE::Vec surfaceNormal(const CORE::Vec &_p) const {
            return CORE::surfaceNormal(_p, [](const CORE::Vec &_x){
                int bulbIterations = 0;
                return UTILS::sdfMandle(_x, bulbIterations);
            });
        }

        // calc surface UV
        CORE::Uv surfaceUv(const CORE::Vec &_p) const {
            return getSphericalUv(_p, _p.size());
        }

     private:
        CORE::Axis             m_axis;
        CORE::Bounds           m_bounds;
        const BASE::Material   *m_pMaterial;
    };

};  // namespace DETAIL


#endif  // #ifndef DETAIL_MARCHED_MANDLE_H

