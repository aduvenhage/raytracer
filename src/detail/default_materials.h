
#ifndef DETAIL_DEFAULT_MATERIALS_H
#define DETAIL_DEFAULT_MATERIALS_H


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
        virtual CORE::ScatteredRay scatter(const BASE::Intersect &_hit) const override {
            auto scatteredDirection = (_hit.m_normal + CORE::randomUnitSphere()).normalized();
            return CORE::ScatteredRay(CORE::Ray(_hit.m_position, scatteredDirection), color(_hit), CORE::Color());
        }
        
     protected:
        /* Returns the diffuse color at the given surface position */
        virtual CORE::Color color(const BASE::Intersect &_hit) const {return m_color;}
        
     private:
        CORE::Color           m_color;
    };


    // diffuse material
    class DiffuseCheckered : public Diffuse
    {
     public:
        DiffuseCheckered(const CORE::Color &_colorA, const CORE::Color &_colorB, int _iBlockSize)
            :Diffuse(CORE::Color()),
             m_colorA(_colorA),
             m_colorB(_colorB),
             m_iBlockSize(_iBlockSize)
        {}
        
        /* Returns the diffuse color at the given surface position */
        virtual CORE::Color color(const BASE::Intersect &_hit) const override {
            float c = (float)(((int)(_hit.m_uv.u() * m_iBlockSize) + (int)(_hit.m_uv.v() * m_iBlockSize)) % 2);
            return m_colorA * c + m_colorB * (1 - c);
        }
        
     private:
        CORE::Color     m_colorA;
        CORE::Color     m_colorB;
        int             m_iBlockSize;
    };


    // diffuse material
    class DiffuseMandlebrot : public Diffuse
    {
     public:
        DiffuseMandlebrot()
            :Diffuse(CORE::Color()),
             m_mandlebrot(1, 1),
             m_baseColor(0.4f, 0.2f, 0.1f)
        {}
        
        /* Returns the diffuse color at the given surface position */
        virtual CORE::Color color(const BASE::Intersect &_hit) const override {
            return m_baseColor * (m_mandlebrot.value(_hit.m_uv.u(), _hit.m_uv.v()) * 0.1f + 0.1f);
        }
        
     private:
        UTILS::MandleBrot     m_mandlebrot;
        CORE::Color           m_baseColor;
    };


    // light emitting material
    class Light : public BASE::Material
    {
     public:
       Light(const CORE::Color &_color)
            :m_color(_color)
       {}
       
       /* Returns the scattered ray at the intersection point. */
       virtual CORE::ScatteredRay scatter(const BASE::Intersect &_hit) const override {
            float fIntensity = fabs(_hit.m_normal * _hit.m_priRay.m_direction);
            return CORE::ScatteredRay(_hit.m_priRay, CORE::Color(), m_color * fIntensity);
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
        virtual CORE::ScatteredRay scatter(const BASE::Intersect &_hit) const override {
            auto normal = (_hit.m_normal + CORE::randomUnitSphere() * m_fScatter).normalized();
            auto reflectedRay = CORE::Ray(_hit.m_position, reflect(_hit.m_priRay.m_direction, normal));
        
            return CORE::ScatteredRay(reflectedRay, m_color, CORE::Color());
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
        virtual CORE::ScatteredRay scatter(const BASE::Intersect &_hit) const override {
            return CORE::ScatteredRay(CORE::Ray(_hit.m_position,
                                                CORE::refract(_hit.m_priRay.m_direction, _hit.m_normal, m_fIndexOfRefraction, _hit.m_bInside, m_fScatter)),
                                                m_color, CORE::Color());
        }

     private:
        CORE::Color    m_color;
        float          m_fScatter;
        float          m_fIndexOfRefraction;
    };
    
    
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
            return CORE::ScatteredRay(scatteredRay, CORE::Color(), color);
        }
    };

};  // namespace DETAIL


#endif  // #ifndef DETAIL_DEFAULT_MATERIALS_H
