
#ifndef DETAIL_BASIC_MATERIALS_H
#define DETAIL_BASIC_MATERIALS_H


#include "core/color.h"
#include "core/ray.h"
#include "base/material.h"
#include "base/intersect.h"
#include "utils/mandlebrot.h"


namespace DETAIL
{
    // diffuse material
    class Diffuse : public BASE::Material
    {
     public:
        Diffuse(const CORE::Color &_color)
            :m_color(_color)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const BASE::Intersect &_hit) const override {
            _sc.m_ray = CORE::Ray(_hit.m_position, CORE::randomUnitSphereOnNormal(_hit.m_normal));
            //_sc.m_ray = CORE::Ray(_hit.m_position, CORE::randomUnitHemisphereOnNormal(_hit.m_normal));
            _sc.m_color *= m_color;
            _sc.m_emitted *= m_color;
            return _sc;
        }
        
     private:
        CORE::Color           m_color;
    };

    
    // Light emitting material
    class Light : public BASE::Material
    {
     public:
       Light(const CORE::Color &_color)
            :m_color(_color)
       {}
       
       /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const BASE::Intersect &_hit) const override {
            _sc.m_ray = CORE::Ray(_hit.m_position, _hit.m_priRay.m_direction);
            if (_hit.m_bInside == false) {
                _sc.m_emitted += m_color * _sc.m_color;
            }
            
            _sc.m_color = CORE::COLOR::Black;

            return _sc;
       }
       
     private:
        CORE::Color            m_color;
    };


    // shiny metal material
    class Metal : public BASE::Material
    {
     public:
        Metal(const CORE::Color &_color, float _fScatter)
            :m_color(_color),
             m_fScatter(_fScatter)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const BASE::Intersect &_hit) const override {
            _sc.m_ray = CORE::Ray(_hit.m_position,
                                  reflect(_hit.m_priRay.m_direction, _hit.m_normal) + CORE::randomInUnitSphere() * m_fScatter);
            _sc.m_color *= m_color;
            _sc.m_emitted *= m_color;
            return _sc;
        }

     private:
        CORE::Color    m_color;
        float          m_fScatter;
    };


    // glass material
    class Glass : public BASE::Material
    {
     public:
        Glass(const CORE::Color &_color, float _fScatter, float _fIndexOfRefraction)
            :m_color(_color),
             m_fScatter(_fScatter),
             m_fIndexOfRefraction(_fIndexOfRefraction)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const BASE::Intersect &_hit) const override {
            _sc.m_ray = CORE::Ray(_hit.m_position,
                                  refract(_hit.m_priRay.m_direction,
                                          (_hit.m_normal + CORE::randomInUnitSphere() * m_fScatter).normalized(),
                                          m_fIndexOfRefraction,
                                          _hit.m_bInside));
            _sc.m_color *= m_color;
            _sc.m_emitted *= m_color;
            return _sc;
        }

     private:
        CORE::Color    m_color;
        float          m_fScatter;
        float          m_fIndexOfRefraction;
    };
    
    
    // coloured according to hit/march depth
    class MarchDepth : public BASE::Material
    {
     public:
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const BASE::Intersect &_hit) const override {
            auto att = CORE::Color(_hit.m_uMarchDepth, _hit.m_uMarchDepth, _hit.m_uMarchDepth);
            _sc.m_color *= att;
            _sc.m_emitted *= att;
            return _sc;
        }
    };


    // coloured according to hit iteration count
    class Iterations : public BASE::Material
    {
     public:
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const BASE::Intersect &_hit) const override {
            auto att = CORE::Color(_hit.m_uIterations, _hit.m_uIterations, _hit.m_uIterations);
            _sc.m_color *= att;
            _sc.m_emitted *= att;
            return _sc;
        }
    };




};  // namespace DETAIL


#endif  // #ifndef DETAIL_BASIC_MATERIALS_H
