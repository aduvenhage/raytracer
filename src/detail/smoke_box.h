#ifndef DETAIL_SMOKE_BOX_H
#define DETAIL_SMOKE_BOX_H

#include "constants.h"
#include "primitive.h"
#include "vec3.h"
#include "uv.h"


namespace DETAIL
{
    /* Axis aligned box shape class -- fixed at origin [0, 0, 0] (with random intersection for smoke, fog, etc.) */
    class SmokeBox        : public Primitive
    {
     public:
        SmokeBox(const Vec &_size, const Material *_pMaterial, float _fVisibility)
            :m_bounds(-_size * 0.5f, _size * 0.5f),
             m_pMaterial(_pMaterial),
             m_fVisibility(_fVisibility)
        {}
        
        SmokeBox(float _fSize, const Material *_pMaterial, float _fVisibility)
            :m_bounds(boxVec(-_fSize*0.5f), boxVec(_fSize*0.5f)),
             m_pMaterial(_pMaterial),
             m_fVisibility(_fVisibility)
        {}
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial;
        }
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit) const override {
            auto bi = aaboxIntersect(m_bounds, _hit.m_priRay);
            if (bi.m_intersect == true) {
                if ( (bi.m_inside == true) ||                    
                     (_hit.m_priRay.inside(bi.m_tmin) == true) )
                {
                    std::uniform_real_distribution<float> dist(0, m_fVisibility);

                    // calculate random hit point on ray inside volume
                    auto tdist = bi.m_inside ? bi.m_tmax : (bi.m_tmax - bi.m_tmin);
                    auto rdist = dist(generator());

                    if (rdist < tdist) {
                        _hit.m_bInside = bi.m_inside;
                        _hit.m_fPositionOnRay = bi.m_inside ? rdist : (bi.m_tmin + rdist);
                        
                        // NOTE: hit normal is opposite to ray direction with some randomness)
                        _hit.m_normal = -(_hit.m_priRay.m_direction + 0.5 * randomUnitSphere()).normalized();

                        return true;
                    }
                }
            }
            
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            _hit.m_position = _hit.m_priRay.position(_hit.m_fPositionOnRay);
            return _hit;
        }
                
        /* returns bounds for shape */
        virtual const Bounds &bounds() const override {
            return  m_bounds;
        }
        
     private:
        Axis                                    m_axis;
        Bounds                                  m_bounds;
        const Material                          *m_pMaterial;
        float                                   m_fVisibility;
    };


};  // namespace DETAIL

#endif  // #ifndef DETAIL_SMOKE_BOX_H

