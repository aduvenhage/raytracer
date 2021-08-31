#ifndef DETAIL_SMOKE_BOX_H
#define DETAIL_SMOKE_BOX_H

#include "core/constants.h"
#include "core/vec3.h"
#include "core/uv.h"
#include "core/random.h"
#include "base/primitive.h"


namespace DETAIL
{
    /*
        Axis aligned box shape class -- fixed at origin [0, 0, 0] (with random intersection for smoke, fog, etc.)
    */
    class SmokeBox        : public BASE::Primitive
    {
     public:
        SmokeBox(const CORE::Vec &_size, const BASE::Material *_pMaterial, float _fVisibility)
            :m_bounds(-_size * 0.5f, _size * 0.5f),
             m_pMaterial(_pMaterial),
             m_fVisibility(_fVisibility)
        {}
        
        SmokeBox(float _fSize, const BASE::Material *_pMaterial, float _fVisibility)
            :m_bounds(CORE::boxVec(-_fSize*0.5f), CORE::boxVec(_fSize*0.5f)),
             m_pMaterial(_pMaterial),
             m_fVisibility(_fVisibility)
        {}
        
        /* Returns the material used for rendering, etc. */
        const BASE::Material *material() const override {
            return m_pMaterial;
        }
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(BASE::Intersect &_hit) const override {
            auto bi = aaboxIntersect(m_bounds, _hit.m_priRay);
            if (bi.m_intersect == true) {
                if ( (bi.m_inside == true) ||                    
                     (_hit.m_priRay.inside(bi.m_tmin) == true) )
                {
                    std::uniform_real_distribution<float> dist(0, m_fVisibility);

                    // calculate random hit point on ray inside volume
                    auto tdist = bi.m_inside ? bi.m_tmax : (bi.m_tmax - bi.m_tmin);
                    auto rdist = dist(CORE::generator());

                    if (rdist < tdist) {
                        _hit.m_bInside = true;
                        _hit.m_fPositionOnRay = bi.m_inside ? rdist : (bi.m_tmin + rdist);
                        return true;
                    }
                }
            }
            
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual BASE::Intersect &intersect(BASE::Intersect &_hit) const override {
            _hit.m_position = _hit.m_priRay.position(_hit.m_fPositionOnRay);
            _hit.m_normal = CORE::randomUnitSphere();

            return _hit;
        }
                
        /* returns bounds for shape */
        virtual const CORE::Bounds &bounds() const override {
            return  m_bounds;
        }
        
     private:
        CORE::Axis                             m_axis;
        CORE::Bounds                           m_bounds;
        const BASE::Material                   *m_pMaterial;
        float                                  m_fVisibility;
    };


};  // namespace DETAIL

#endif  // #ifndef DETAIL_SMOKE_BOX_H

