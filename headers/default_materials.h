
#ifndef LIBS_HEADER_DEFAULT_MATERIALS_H
#define LIBS_HEADER_DEFAULT_MATERIALS_H


#include "material.h"
#include "color.h"
#include "intersect.h"
#include "ray.h"


namespace LNF
{
    // diffuse material
    class Diffuse : public Material
    {
     public:
        Diffuse(const Color &_color)
            :m_color(_color)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, const Ray &_ray, RandomGen &_randomGen) const override {
            auto scatteredDirection = (_hit.m_normal + randomUnitSphere(_randomGen)).normalized();
            return ScatteredRay(Ray(_hit.m_position, scatteredDirection), color(_hit), Color());
        }
        
     protected:
        /* Returns the diffuse color at the given surface position */
        virtual Color color(const Intersect &_hit) const {return m_color;}
        
     private:
        Color           m_color;
    };


    // diffuse material
    class DiffuseCheckered : public Diffuse
    {
     public:
        DiffuseCheckered(const Color &_colorA, const Color &_colorB, int _iBlockSize)
            :Diffuse(Color()),
             m_colorA(_colorA),
             m_colorB(_colorB),
             m_iBlockSize(_iBlockSize)
        {}
        
        /* Returns the diffuse color at the given surface position */
        virtual Color color(const Intersect &_hit) const override {
            int c = ((int)(_hit.m_uv.m_dU * m_iBlockSize) + (int)(_hit.m_uv.m_dV * m_iBlockSize)) % 2;
            return m_colorA * c + m_colorB * (1 - c);
        }
        
     private:
        Color           m_colorA;
        Color           m_colorB;
        int             m_iBlockSize;
    };


    // light emitting material
    class Light : public Material
    {
     public:
       Light(const Color &_color)
           :m_color(_color)
       {}
       
       /* Returns the scattered ray at the intersection point. */
       virtual ScatteredRay scatter(const Intersect &_hit, const Ray &_ray, RandomGen &_randomGen) const override {
           return ScatteredRay(_ray, Color(), m_color);
       }
       
     private:
       Color            m_color;
    };


    // shiny metal material
    class Metal : public Material
    {
     public:
        Metal(const Color &_color, float _dScatter)
            :m_color(_color),
             m_dScatter(_dScatter)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, const Ray &_ray, RandomGen &_randomGen) const override {
            auto normal = (_hit.m_normal + randomUnitSphere(_randomGen) * m_dScatter).normalized();
            auto reflectedRay = Ray(_hit.m_position, reflect(_ray.m_direction, normal));
        
            return ScatteredRay(reflectedRay, m_color, Color());
        }

     private:
        Color          m_color;
        float          m_dScatter;
    };


    // glass material
    class Glass : public Material
    {
     public:
        Glass(const Color &_color, float _dScatter, float _dIndexOfRefraction)
            :m_color(_color),
             m_dScatter(_dScatter),
             m_dIndexOfRefraction(_dIndexOfRefraction)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, const Ray &_ray, RandomGen &_randomGen) const override {
            auto normal = (_hit.m_normal + randomUnitSphere(_randomGen) * m_dScatter).normalized();
            float dEtaiOverEtat = 1.0f/m_dIndexOfRefraction;
            
            if (_hit.m_bInside == true) {
                normal = -normal;
                dEtaiOverEtat = m_dIndexOfRefraction;
            }
            
            auto refractedRay = Ray(_hit.m_position, refract(_ray.m_direction, normal, dEtaiOverEtat, _randomGen));
            return ScatteredRay(refractedRay, m_color, Color());
        }

     private:
        Color          m_color;
        float          m_dScatter;
        float          m_dIndexOfRefraction;
    };
    
};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_DEFAULT_MATERIALS_H
