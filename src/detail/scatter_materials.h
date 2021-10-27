
#ifndef DETAIL_SCATTER_MATERIALS_H
#define DETAIL_SCATTER_MATERIALS_H


#include "core/color.h"
#include "core/ray.h"
#include "core/random.h"
#include "base/material.h"
#include "base/intersect.h"
#include "utils/mandlebrot.h"


namespace DETAIL
{
    // material with internal scattering
    class Scatter : public BASE::Material
    {
     public:
        Scatter(const CORE::Color &_color)
            :m_color(_color)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const BASE::Intersect &_hit) const override {
            std::uniform_real_distribution<float> dist01(0, 1);
            
            // reflect or scatter when hit from outside
            if (_hit.m_bInside == false) {
                if (dist01(CORE::generator()) < 0.2) {
                    _sc.m_ray = CORE::Ray(_hit.m_position,
                                          (reflect(_hit.m_priRay.m_direction, _hit.m_normal) + CORE::randomInUnitSphere() * 0.4).normalized());
                    _sc.m_color *= m_color;
                    _sc.m_emitted *= m_color;
                    return _sc;
                }
                else {
                    _sc.m_ray = CORE::Ray(_hit.m_position, (_hit.m_priRay.m_direction + CORE::randomInUnitSphere() * 0.4).normalized());
                    _sc.m_color *= m_color;
                    _sc.m_emitted *= m_color;
                    return _sc;
                }
            }
            
            // scatter when hit from inside
            else {
                _sc.m_ray = CORE::Ray(_hit.m_position, CORE::randomOnUnitSphere());
                _sc.m_color *= m_color;
                _sc.m_emitted *= m_color;
                return _sc;
            }
        }

     private:
        float distance() const {
            const float density = 0.5f;
            const float negInvDensity = -1.0f / density;
            std::uniform_real_distribution<float> dist(0, 1);
            return negInvDensity * log(dist(CORE::generator()));
        }

     private:
        CORE::Color    m_color;
    };

};  // namespace DETAIL


#endif  // #ifndef DETAIL_SCATTER_MATERIALS_H
