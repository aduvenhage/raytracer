
#ifndef DETAIL_SPECIAL_MATERIALS_H
#define DETAIL_SPECIAL_MATERIALS_H


#include "core/color.h"
#include "core/ray.h"
#include "base/material.h"
#include "base/intersect.h"


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
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const BASE::Intersect &_hit) const override {
            if (_hit.m_bInside == m_bInside) {
                _sc.m_ray = CORE::Ray(_hit.m_position, randomUnitSphereOnNormal(_hit.m_normal));
                _sc.m_color = CORE::COLOR::Black;
                _sc.m_emitted = CORE::Color((_hit.m_normal.x() + 1)/2, (_hit.m_normal.y() + 1)/2, (_hit.m_normal.z() + 1)/2);
            }
            else {
                _sc.m_ray = CORE::Ray(_hit.m_position, _hit.m_priRay.m_direction);
                _sc.m_color = CORE::COLOR::White;
                _sc.m_emitted = CORE::COLOR::Black;
            }

            return _sc;
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
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const BASE::Intersect &_hit) const override {
            _sc.m_ray = CORE::Ray(_hit.m_position, randomUnitSphereOnNormal(_hit.m_normal));
            _sc.m_color = CORE::COLOR::Black;
            _sc.m_emitted = CORE::COLOR::Red * _hit.m_uv.u() + CORE::COLOR::Green * _hit.m_uv.v() + CORE::COLOR::Blue * (1 - _hit.m_uv.u() - _hit.m_uv.v());
            return _sc;
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
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const BASE::Intersect &_hit) const override {
            _sc.m_ray = CORE::Ray(_hit.m_position, _hit.m_normal);
            _sc.m_color *= m_color;
            return _sc;
        }
        
     private:
        CORE::Color           m_color;
    };


};  // namespace DETAIL


#endif  // #ifndef DETAIL_SPECIAL_MATERIALS_H
