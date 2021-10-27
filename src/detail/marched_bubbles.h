#ifndef DETAIL_MARCHED_BUBBLES_H
#define DETAIL_MARCHED_BUBBLES_H

#include "core/constants.h"
#include "core/uv.h"
#include "core/vec3.h"
#include "base/primitive.h"
#include "systems/trace.h"
#include "utils/signed_distance_functions.h"


namespace DETAIL
{
    class MarchedBubbles        : public BASE::Primitive
    {
     public:
        MarchedBubbles(const CORE::Vec &_size, const BASE::Material *_pMaterial)
            :m_bounds(-_size * 0.5f, _size * 0.5f),
             m_pMaterial(_pMaterial),
             m_fSize(_size.size() * 0.5f)
        {}

        MarchedBubbles(float _fSize, const BASE::Material *_pMaterial)
            :m_bounds(CORE::boxVec(-_fSize*0.5f), CORE::boxVec(_fSize*0.5f)),
             m_pMaterial(_pMaterial),
             m_fSize(_fSize * 0.5f)
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
                                                            return UTILS::sdfBubbles(_p, 0, m_fSize*2.0f);
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
                return UTILS::sdfBubbles(_x, 0, m_fSize * 2.0f);
            });
        }

        // calc surface UV
        CORE::Uv surfaceUv(const CORE::Vec &_n) const {
            return getSphericalUv(_n);
        }

     private:
        CORE::Axis             m_axis;
        CORE::Bounds           m_bounds;
        const BASE::Material   *m_pMaterial;
        float                  m_fSize;
    };

};  // namespace DETAIL


#endif  // #ifndef DETAIL_MARCHED_BUBBLES_H

