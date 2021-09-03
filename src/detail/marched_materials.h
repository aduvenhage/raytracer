
#ifndef DETAIL_MARCHED_MATERIALS_H
#define DETAIL_MARCHED_MATERIALS_H


#include "core/color.h"
#include "core/ray.h"
#include "base/material.h"
#include "base/intersect.h"


namespace DETAIL
{
    // diffuse and coloured according to hit iteration count
    class FakeAmbientOcclusion : public BASE::Material
    {
     public:
        FakeAmbientOcclusion()
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay scatter(const BASE::Intersect &_hit) const override {
            auto color = CORE::Color(_hit.m_uMarchDepth*0.005f,
                                     _hit.m_uMarchDepth*0.005f,
                                     _hit.m_uMarchDepth*0.005f,
                                     CORE::Color::OPERATION::CLAMP);
            
            return CORE::ScatteredRay(CORE::Ray(_hit.m_position, randomUnitSphereOnNormal(_hit.m_normal)),
                                      color, CORE::Color());
        }

     private:
    };


    // metal and coloured according to hit iteration count
    class MetalIterations : public BASE::Material
    {
     public:
        MetalIterations()
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay scatter(const BASE::Intersect &_hit) const override {
            float scale = 3.0f;
            auto color = CORE::Color(_hit.m_uMarchDepth*0.25f*scale,
                                     _hit.m_uMarchDepth*0.13f*scale,
                                     _hit.m_uMarchDepth*0.07f*scale,
                                     CORE::Color::OPERATION::WRAP);

            return CORE::ScatteredRay(CORE::Ray(_hit.m_position, randomUnitSphereOnNormal(_hit.m_normal)),
                                      color, CORE::Color());
        }

     private:
    };


    // diffuse and coloured according to hit iteration count
    class Glow : public BASE::Material
    {
     public:
        Glow()
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay scatter(const BASE::Intersect &_hit) const override {
            auto color = (CORE::Color(0.0f, 0.21f, 0.11f) * _hit.m_uIterations).wrap();
            auto glow = (CORE::Color(1.0f, 0.0f, 0.0f) - CORE::Color(0.01f, 0.0f, 0.0f) * _hit.m_uMarchDepth).clamp() * 10;
            
            return CORE::ScatteredRay(CORE::Ray(_hit.m_position, randomUnitSphereOnNormal(_hit.m_normal)),
                                      color, glow);
        }

     private:
    };


};  // namespace DETAIL


#endif  // #ifndef DETAIL_MARCHED_MATERIALS_H

