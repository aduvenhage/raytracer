
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
        DiffuseMandlebrot(const CORE::Color &_baseColor, double _fCx, double _fCy, double _fZoom, int _iMaxIterations = 0)
            :Diffuse(CORE::Color()),
             m_mandlebrot(1, 1),
             m_baseColor(_baseColor)
        {
            int maxIterations = _iMaxIterations > 0 ? _iMaxIterations : (int)(_fZoom * 50 + 5);
            m_mandlebrot.setView(_fCx, _fCy, _fZoom, maxIterations);
        }
        
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
        LightMandlebrot(const CORE::Color &_baseColor, double _fCx, double _fCy, double _fZoom, int _iMaxIterations = 0)
            :Light(CORE::Color()),
             m_mandlebrot(1, 1),
             m_baseColor(_baseColor)
        {
            int maxIterations = _iMaxIterations > 0 ? _iMaxIterations : (int)(_fZoom * 50 + 5);
            m_mandlebrot.setView(_fCx, _fCy, _fZoom, maxIterations);
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
