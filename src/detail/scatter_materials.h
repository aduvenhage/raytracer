
#ifndef DETAIL_SCATTER_MATERIALS_H
#define DETAIL_SCATTER_MATERIALS_H


#include "core/color.h"
#include "core/ray.h"
#include "core/random.h"
#include "base/material.h"
#include "base/intersect.h"
#include "utils/mandlebrot.h"


namespace DETAIL
{
    // glass material with internal scattering
    class GlassScatter : public BASE::Material
    {
     public:
        GlassScatter(const CORE::Color &_color, float _fScatter, float _fIndexOfRefraction)
            :m_color(_color),
             m_fScatter(_fScatter),
             m_fIndexOfRefraction(_fIndexOfRefraction)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const BASE::Intersect &_hit) const override {
            // TODO: implement fog
            return _sc;
        }

     private:
        CORE::Color    m_color;
        float          m_fScatter;
        float          m_fIndexOfRefraction;
    };

};  // namespace DETAIL


#endif  // #ifndef DETAIL_SCATTER_MATERIALS_H
