
#ifndef DETAIL_SPECIAL_MATERIALS_H
#define DETAIL_SPECIAL_MATERIALS_H


#include "core/color.h"
#include "core/ray.h"
#include "base/material.h"
#include "base/intersect.h"
#include "utils/mandlebrot.h"


namespace DETAIL
{
    // material that colors surface based on surface normal
    class SurfaceNormal : public BASE::Material
    {
     public:
        SurfaceNormal(bool _bInside=false)
            :m_bInside(_bInside)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay scatter(const BASE::Intersect &_hit) const override {
            if (_hit.m_bInside == m_bInside) {
                auto scatteredDirection = (_hit.m_normal + CORE::randomUnitSphere()).normalized();
                auto scatteredRay = CORE::Ray(_hit.m_position, scatteredDirection);
                auto color = CORE::Color((_hit.m_normal.x() + 1)/2, (_hit.m_normal.y() + 1)/2, (_hit.m_normal.z() + 1)/2);

                return CORE::ScatteredRay(scatteredRay, CORE::Color(), color);
            }
            else {
                auto passThroughRay = CORE::Ray(_hit.m_position, _hit.m_priRay.m_direction);
                return CORE::ScatteredRay(passThroughRay, CORE::Color(1, 1, 1), CORE::Color());
            }
        }

     private:
        bool           m_bInside;
    };


    // triangle tri-color surface
    class TriangleRGB    : public BASE::Material
    {
     public:
        TriangleRGB()
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay scatter(const BASE::Intersect &_hit) const override {
            auto scatteredDirection = (_hit.m_normal + CORE::randomUnitSphere()).normalized();
            auto scatteredRay = CORE::Ray(_hit.m_position, scatteredDirection);
            
            auto color = CORE::COLOR::Red * _hit.m_uv.u() + CORE::COLOR::Green * _hit.m_uv.v() + CORE::COLOR::Blue * (1 - _hit.m_uv.u() - _hit.m_uv.v());
            return CORE::ScatteredRay(scatteredRay, CORE::Color(), color.clamp());
        }
    };

};  // namespace DETAIL


#endif  // #ifndef DETAIL_SPECIAL_MATERIALS_H
