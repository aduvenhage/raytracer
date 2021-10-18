
#ifndef DETAIL_TEX_MATERIALS_H
#define DETAIL_TEX_MATERIALS_H


#include "core/color.h"
#include "core/ray.h"
#include "base/material.h"
#include "base/intersect.h"
#include "utils/mandlebrot.h"
#include "core/constants.h"


namespace DETAIL
{
    // checkered texture material
    class Checkered : public BASE::Material
    {
     public:
        Checkered(CORE::Color &_c1, CORE::Color &_c2, int _iBlockSize)
            :m_color1(_c1),
             m_color2(_c2),
             m_fScale(_iBlockSize * pi)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const BASE::Intersect &_hit) const override {
            float v = sin(_hit.m_uv.u() * m_fScale) * sin(_hit.m_uv.v() * m_fScale);
            auto att = v > 0 ? m_color1 : m_color2;
            _sc.m_color *= att;
            _sc.m_emitted *= att;
            return _sc;
        }
        
     private:
        CORE::Color     m_color1;
        CORE::Color     m_color2;
        float           m_fScale;
    };


    // mandlebrot coloured
    class Mandlebrot : public BASE::Material
    {
     public:
        Mandlebrot(const CORE::Color &_baseColor, const CORE::Color &_offsetColor, float _fBrightness,
                   double _fCx, double _fCy, double _fZoom, int _iMaxIterations = 0)
            :m_mandlebrot(1, 1),
             m_baseColor(_baseColor),
             m_offsetColor(_offsetColor),
             m_fBrightness(_fBrightness)
        {
            int maxIterations = _iMaxIterations > 0 ? _iMaxIterations : (int)(_fZoom * 50 + 5);
            m_mandlebrot.setView(_fCx, _fCy, _fZoom, maxIterations);
        }

        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const BASE::Intersect &_hit) const override {
            auto att = (m_baseColor * m_mandlebrot.value(_hit.m_uv.u(), _hit.m_uv.v())).wrap() * m_fBrightness + m_offsetColor;
            _sc.m_color *= att.clamp();
            _sc.m_emitted *= att;
            return _sc;
        }
        
     private:
        UTILS::MandleBrot     m_mandlebrot;
        CORE::Color           m_baseColor;
        CORE::Color           m_offsetColor;
        float                 m_fBrightness;
    };
};  // namespace DETAIL


#endif  // #ifndef DETAIL_TEX_MATERIALS_H
