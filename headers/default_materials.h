
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
        virtual ScatteredRay scatter(const Intersect &_hit, RandomGen &_randomGen) const override {
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
            int c = ((int)(_hit.m_uv.m_fU * m_iBlockSize) + (int)(_hit.m_uv.m_fV * m_iBlockSize)) % 2;
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
       virtual ScatteredRay scatter(const Intersect &_hit, RandomGen &_randomGen) const override {
           return ScatteredRay(_hit.m_ray, Color(), m_color);
       }
       
     private:
       Color            m_color;
    };


    // shiny metal material
    class Metal : public Material
    {
     public:
        Metal(const Color &_color, float _fScatter)
            :m_color(_color),
             m_fScatter(_fScatter)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, RandomGen &_randomGen) const override {
            auto normal = (_hit.m_normal + randomUnitSphere(_randomGen) * m_fScatter).normalized();
            auto reflectedRay = Ray(_hit.m_position, reflect(_hit.m_ray.m_direction, normal));
        
            return ScatteredRay(reflectedRay, m_color, Color());
        }

     private:
        Color          m_color;
        float          m_fScatter;
    };


    // glass material
    class Glass : public Material
    {
     public:
        Glass(const Color &_color, float _fScatter, float _fIndexOfRefraction)
            :m_color(_color),
             m_fScatter(_fScatter),
             m_fIndexOfRefraction(_fIndexOfRefraction)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, RandomGen &_randomGen) const override {
            float dEtaiOverEtat = _hit.m_bInside ? m_fIndexOfRefraction : (1.0f/m_fIndexOfRefraction);
            auto normal = (_hit.m_normal + randomUnitSphere(_randomGen) * m_fScatter).normalized();
            normal *= _hit.m_bInside ? -1 : 1;
            
            auto refractedRay = Ray(_hit.m_position, refract(_hit.m_ray.m_direction, normal, dEtaiOverEtat, _randomGen));
            return ScatteredRay(refractedRay, m_color, Color());
        }

     private:
        Color          m_color;
        float          m_fScatter;
        float          m_fIndexOfRefraction;
    };
    
    
    // material that colors surface based on surface normal
    class SurfaceNormal : public Material
    {
     public:
        SurfaceNormal(bool _bInside=false)
            :m_bInside(_bInside)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, RandomGen &_randomGen) const override {
            if (_hit.m_bInside == m_bInside) {
                auto scatteredDirection = (_hit.m_normal + randomUnitSphere(_randomGen)).normalized();
                auto scatteredRay = Ray(_hit.m_position, scatteredDirection);
                auto color = Color((_hit.m_normal.m_fX + 1)/2, (_hit.m_normal.m_fY + 1)/2, (_hit.m_normal.m_fZ + 1)/2);

                return ScatteredRay(scatteredRay, Color(), color);
            }
            else {
                auto passThroughRay = Ray(_hit.m_position, _hit.m_ray.m_direction);
                return ScatteredRay(passThroughRay, Color(1, 1, 1), Color());
            }
        }

     private:
        bool           m_bInside;
    };
    
    
    
    
    
    // DEBUGGING -- draw normals


    
};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_DEFAULT_MATERIALS_H
