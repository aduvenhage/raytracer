
#ifndef DETAIL_FRACTAL_MATERIALS_H
#define DETAIL_FRACTAL_MATERIALS_H


#include "core/color.h"
#include "core/ray.h"
#include "base/material.h"
#include "base/intersect.h"
#include "utils/mandlebrot.h"
#include "basic_materials.h"


namespace DETAIL
{
    // mandlebrot diffuse
    class DiffuseMandlebrot : public Diffuse
    {
     public:
        DiffuseMandlebrot()
            :Diffuse(CORE::Color()),
             m_mandlebrot(1, 1),
             m_baseColor(0.02f, 0.05f, 0.06f)
        {}
        
        /* Returns the diffuse color at the given surface position */
        virtual CORE::Color color(const BASE::Intersect &_hit) const override {
            return m_baseColor * m_mandlebrot.value(_hit.m_uv.u(), _hit.m_uv.v());
        }
        
     private:
        UTILS::MandleBrot     m_mandlebrot;
        CORE::Color           m_baseColor;
    };


    // mandlebrot light
    class LightMandlebrot : public Light
    {
     public:
        LightMandlebrot()
            :Light(CORE::Color()),
             m_mandlebrot(1, 1),
             m_baseColor(0.002f, 0.0024f, 0.0006f)
        {
            m_mandlebrot.setView(-0.7453, 0.1127, 180.0f, 5000.0f);
        }
        
        /* Returns the diffuse color at the given surface position */
        virtual CORE::Color color(const BASE::Intersect &_hit) const override {
            auto color = m_baseColor * m_mandlebrot.value(_hit.m_uv.u(), _hit.m_uv.v());
            return color.wrap() + CORE::Color(0.1, 0.1, 0.1);
        }
        
     private:
        UTILS::MandleBrot     m_mandlebrot;
        CORE::Color           m_baseColor;
    };


};  // namespace DETAIL


#endif  // #ifndef DETAIL_FRACTAL_MATERIALS_H
