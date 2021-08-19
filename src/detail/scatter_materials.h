
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
    // glass material with internal scattering
    class GlassScatter : public BASE::Material
    {
     public:
        GlassScatter(const CORE::Color &_color, float _fScatter, float _fIndexOfRefraction)
            :m_color(_color),
             m_fScatter(_fScatter),
             m_fIndexOfRefraction(_fIndexOfRefraction)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay scatter(const BASE::Intersect &_hit) const override {
            if (_hit.m_bInside == true) {
                std::uniform_real_distribution<float> uniform01(0, 1);
        
                if (uniform01(CORE::generator()) <= 0.6) {
                    auto position = _hit.m_priRay.position(_hit.m_fPositionOnRay * 0.98);
                    auto scatteredDirection = (_hit.m_normal + CORE::randomUnitSphere() * 0.5).normalized();
                    
                    return CORE::ScatteredRay(CORE::Ray(position, scatteredDirection), m_color, CORE::Color());
                }
            }

            return CORE::ScatteredRay(CORE::Ray(_hit.m_position,
                                                CORE::refract(_hit.m_priRay.m_direction, _hit.m_normal, m_fIndexOfRefraction, _hit.m_bInside, m_fScatter)),
                                                m_color, CORE::Color());
        }

     private:
        CORE::Color    m_color;
        float          m_fScatter;
        float          m_fIndexOfRefraction;
    };

};  // namespace DETAIL


#endif  // #ifndef DETAIL_SCATTER_MATERIALS_H
