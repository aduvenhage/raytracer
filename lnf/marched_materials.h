
#ifndef LIBS_HEADER_MARCHED_MATERIALS_H
#define LIBS_HEADER_MARCHED_MATERIALS_H


#include "material.h"
#include "color.h"
#include "intersect.h"
#include "ray.h"


namespace LNF
{
    // diffuse and coloured according to hit iteration count
    class FakeAmbientOcclusion : public Material
    {
     public:
        FakeAmbientOcclusion()
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, RandomGen &_randomGen) const override {
            auto color = Color(_hit.m_uIterationCount*0.005f,
                               _hit.m_uIterationCount*0.005f,
                               _hit.m_uIterationCount*0.005f).clamp();
            
            auto scatteredDirection = (_hit.m_normal + randomUnitSphere(_randomGen)).normalized();
            return ScatteredRay(Ray(_hit.m_position, scatteredDirection), color, Color());
        }

     private:
    };


    // metal and coloured according to hit iteration count
    class MetalIterations : public Material
    {
     public:
        MetalIterations()
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, RandomGen &_randomGen) const override {
            float scale = 3.0f;
            auto color = Color(_hit.m_uIterationCount*0.25f*scale,
                               _hit.m_uIterationCount*0.13f*scale,
                               _hit.m_uIterationCount*0.07f*scale).wrap();
            
            auto normal = (_hit.m_normal + randomUnitSphere(_randomGen) * 0.05).normalized();
            auto reflectedRay = Ray(_hit.m_position, reflect(_hit.m_ray.m_direction, normal));
        
            return ScatteredRay(reflectedRay, color, Color());
        }

     private:
    };


    // diffuse and coloured according to hit iteration count
    class Glow : public Material
    {
     public:
        Glow()
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, RandomGen &_randomGen) const override {
            auto color = Color(_hit.m_uIterationCount*0.0f,
                               _hit.m_uIterationCount*0.02f,
                               _hit.m_uIterationCount*0.015f).wrap();
            
            auto glow = Color((1.0f - _hit.m_uIterationCount*0.01f), 0.0f, 0.0f).clamp() * 10;
            
            auto scatteredDirection = (_hit.m_normal + randomUnitSphere(_randomGen)).normalized();
            return ScatteredRay(Ray(_hit.m_position, scatteredDirection), color, glow);
        }

     private:
    };


};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_MARCHED_MATERIALS_H
