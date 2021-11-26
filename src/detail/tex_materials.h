
#ifndef DETAIL_TEX_MATERIALS_H
#define DETAIL_TEX_MATERIALS_H


#include "core/constants.h"
#include "core/color.h"
#include "core/ray.h"
#include "core/image.h"
#include "base/material.h"
#include "base/intersect.h"
#include "mandlebrot.h"


namespace DETAIL
{
    // checkered texture material
    class Checkered : public BASE::Material
    {
     public:
        Checkered(CORE::Color &_c1, CORE::Color &_c2, int _iBlockSize)
            :m_color1(_c1),
             m_color2(_c2),
             m_fScale(_iBlockSize * pif)
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
            auto att = (m_baseColor * (float)m_mandlebrot.value(_hit.m_uv.u(), _hit.m_uv.v())).wrap() * m_fBrightness + m_offsetColor;
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


    // texture
    class Image : public BASE::Material
    {
     public:
        Image(const char *_pszImagePath)
            :m_pData(nullptr),
             m_iWidth(0),
             m_iHeight(0),
             m_iBytesPerPixel(3),
             m_iBytesPerLine(0)
        {
            auto image = CORE::loadImageFile(_pszImagePath);
            m_pData = image.m_pData;
            m_iWidth = image.m_iWidth;
            m_iHeight = image.m_iHeight;
            m_iBytesPerPixel = image.m_iBytesPerPixel;
            m_iBytesPerLine = m_iBytesPerPixel * m_iWidth;
        }
        
        ~Image() {
           delete m_pData;
        }

        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const BASE::Intersect &_hit) const override {
            if (m_pData != nullptr) {
                const auto uv = CORE::Uv(_hit.m_uv).wrap();
                const unsigned char *pPixel = m_pData +
                                             (int)(m_iWidth * uv.u() + 0.5) * m_iBytesPerPixel +
                                             (int)(m_iHeight * uv.v() + 0.5) * m_iBytesPerLine;

                const auto att = CORE::Color(pPixel[0]/255.0f, pPixel[1]/255.0f, pPixel[2]/255.0f);
                
                _sc.m_color *= att;
                _sc.m_emitted *= att;
                return _sc;
            }
            else {
                const float scale = 32.0f;
                const float v = sin(_hit.m_uv.u() * scale) * sin(_hit.m_uv.v() * scale);
                const auto att = v > 0 ? CORE::COLOR::White : CORE::COLOR::Black;
                _sc.m_color *= att;
                _sc.m_emitted *= att;
                return _sc;
            }
        }
        
     private:
        unsigned char       *m_pData;
        int                 m_iWidth, m_iHeight;
        int                 m_iBytesPerPixel;
        int                 m_iBytesPerLine;
    };


};  // namespace DETAIL


#endif  // #ifndef DETAIL_TEX_MATERIALS_H
