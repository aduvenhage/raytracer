
#ifndef DETAIL_FRACTAL_MATERIALS_H
#define DETAIL_FRACTAL_MATERIALS_H


#include "core/color.h"
#include "core/ray.h"
#include "base/material.h"
#include "base/intersect.h"
#include "utils/mandlebrot.h"


namespace DETAIL
{
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


};  // namespace DETAIL


#endif  // #ifndef DETAIL_FRACTAL_MATERIALS_H
