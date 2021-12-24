#pragma once

#include "core/constants.h"
#include "core/uv.h"
#include "core/vec3.h"
#include "base/primitive.h"
#include "systems/trace.h"
#include "signed_distance_functions.h"


namespace DETAIL
{
    class MarchedBlob        : public BASE::Primitive
    {
     public:
        MarchedBlob(const CORE::Vec &_size, const BASE::Material *_pMaterial, float _fRouhgness)
            :m_bounds(-_size * 0.5f, _size * 0.5f),
             m_pMaterial(_pMaterial),
             m_fSize(_size.size() * 0.5f),
             m_fRouhgness(_fRouhgness)
        {}

        MarchedBlob(float _fSize, const BASE::Material *_pMaterial, float _fRouhgness)
            :m_bounds(CORE::boxVec(-_fSize*0.5f), CORE::boxVec(_fSize*0.5f)),
             m_pMaterial(_pMaterial),
             m_fSize(_fSize * 0.5f),
             m_fRouhgness(_fRouhgness)
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
                                                            return sdf(_p);
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
                return sdf(_x);
            });
        }

        // calc surface UV
        CORE::Uv surfaceUv(const CORE::Vec &_n) const {
            return getSphericalUv(_n);
        }
        
        // calc sdf
        float sdf(const CORE::Vec &_x) const {
            return _x.size() - m_fSize +
                   m_fRouhgness * sin(_x.x()/m_fSize*8) * sin(_x.y()/m_fSize*8) * sin(_x.z()/m_fSize*8) +
                   0.2f * m_fRouhgness * sin(_x.x()/m_fSize*16) * sin(_x.y()/m_fSize*16) * sin(_x.z()/m_fSize*16);
        }

     private:
        CORE::Axis             m_axis;
        CORE::Bounds           m_bounds;
        const BASE::Material   *m_pMaterial;
        float                  m_fSize;
        float                  m_fRouhgness;
    };

};  // namespace DETAIL


