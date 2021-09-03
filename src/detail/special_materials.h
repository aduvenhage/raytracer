
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
                auto color = CORE::Color((_hit.m_normal.x() + 1)/2, (_hit.m_normal.y() + 1)/2, (_hit.m_normal.z() + 1)/2);
                return CORE::ScatteredRay(CORE::Ray(_hit.m_position, randomUnitSphereOnNormal(_hit.m_normal)),
                                          CORE::Color(), color);
            }
            else {
                return CORE::ScatteredRay(CORE::Ray(_hit.m_position, _hit.m_priRay.m_direction),
                                          CORE::Color(1, 1, 1), CORE::Color());
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
            auto color = CORE::COLOR::Red * _hit.m_uv.u() + CORE::COLOR::Green * _hit.m_uv.v() + CORE::COLOR::Blue * (1 - _hit.m_uv.u() - _hit.m_uv.v());
            return CORE::ScatteredRay(CORE::Ray(_hit.m_position, randomUnitSphereOnNormal(_hit.m_normal)),
                                      CORE::Color(), color);
        }
    };
    
    
    // environment map (scatters light in hit point surface normal direction)
    class EnvironmentMap    : public BASE::Material
    {
     public:
        EnvironmentMap(const CORE::Color &_color)
            :m_color(_color)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay scatter(const BASE::Intersect &_hit) const override {
            return CORE::ScatteredRay(CORE::Ray(_hit.m_position, _hit.m_normal), color(_hit), CORE::Color());
        }
        
     protected:
        /* Returns the diffuse color at the given surface position */
        virtual CORE::Color color(const BASE::Intersect &_hit) const {return m_color;}
        
     private:
        CORE::Color           m_color;
    };


};  // namespace DETAIL


#endif  // #ifndef DETAIL_SPECIAL_MATERIALS_H
