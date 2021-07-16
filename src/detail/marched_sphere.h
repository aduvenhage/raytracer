#ifndef DETAIL_MARCHED_SPHERE_H
#define DETAIL_MARCHED_SPHERE_H

#include "core/constants.h"
#include "core/uv.h"
#include "core/vec3.h"
#include "base/primitive.h"
#include "systems/trace.h"
#include "utils/signed_distance_functions.h"


namespace DETAIL
{
    /* Axis aligned box shape class -- fixed at origin [0, 0, 0]; base class for raymarched nodes */
    class MarchedSphere        : public BASE::Primitive
    {
     public:
        MarchedSphere(const CORE::Vec &_size, const BASE::Material *_pMaterial, float _fWaveRatio)
            :m_bounds(-_size * 0.5f, _size * 0.5f),
             m_pMaterial(_pMaterial),
             m_fSize(_size.size() * 0.5f),
             m_fWaveRatio(_fWaveRatio)
        {}

        MarchedSphere(float _fSize, const BASE::Material *_pMaterial, float _fWaveRatio)
            :m_bounds(CORE::boxVec(-_fSize*0.5f), CORE::boxVec(_fSize*0.5f)),
             m_pMaterial(_pMaterial),
             m_fSize(_fSize * 0.5f),
             m_fWaveRatio(_fWaveRatio)
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
                                                            return UTILS::sdfSphereDeformed(_p,
                                                                                            m_fSize * (1 - m_fWaveRatio),
                                                                                            m_fSize * m_fWaveRatio);
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
            return CORE::surfaceNormal(_p, [this](const CORE::Vec &_x){
                return UTILS::sdfSphereDeformed(_x,
                                                m_fSize * (1 - m_fWaveRatio),
                                                m_fSize * m_fWaveRatio);
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
        float                  m_fSize;
        float                  m_fWaveRatio;
    };

};  // namespace DETAIL


#endif  // #ifndef DETAIL_MARCHED_SPHERE_H

