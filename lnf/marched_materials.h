
#ifndef LIBS_HEADER_MARCHED_MATERIALS_H
#define LIBS_HEADER_MARCHED_MATERIALS_H


#include "material.h"
#include "color.h"
#include "intersect.h"
#include "ray.h"


namespace LNF
{
    /*
        Glass material with ray marched surface inside.
    */
    class MarchedGlass : public Material
    {
     public:
        MarchedGlass(float _fGlassScatter, float _fSurfaceScatter, float _fIndexOfRefraction)
            :m_fGlassScatter(_fGlassScatter),
             m_fSurfaceScatter(_fSurfaceScatter),
             m_fIndexOfRefraction(_fIndexOfRefraction)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, RandomGen &_randomGen) const override {
            // reflect/refract through glass
            auto ray = refract(_hit.m_ray.m_direction, _hit.m_normal,
                               m_fIndexOfRefraction, _hit.m_bInside, m_fGlassScatter, _randomGen);
            if (_hit.m_bInside == false) {
                // try to hit surface inside (using signed distance function)
                Vec pos, normal;
                bool bHit = marchedTrace(pos, normal,
                                         Ray(_hit.m_position, ray),
                                         [this](const Vec &_p){return this->sdfSurface(_p);},
                                         0.2,
                                         50);
                                         
                if (bHit == true) {
                    // get inside surface color
                    Color emittedColor;
                    Color diffuseColor;
                    colorSurface(diffuseColor, emittedColor, pos);
                    
                    // calc surface normal and reflect off inside surface
                    auto surfaceNormal = (normal + randomUnitSphere(_randomGen) * m_fSurfaceScatter).normalized();
                    return ScatteredRay(Ray(pos, reflect(ray, surfaceNormal)),
                                        diffuseColor,
                                        emittedColor);
                }
            }

            // no inner hit, just return refracted glass ray
            return ScatteredRay(Ray(_hit.m_position, ray),
                                Color(1.0f, 1.0f, 1.0f),
                                Color());
        }
        
     protected:
        // calc surface color
        virtual void colorSurface(Color &_diffuse, Color &_emitted, const Vec &_p) const = 0;
        
        // surface signed distance function
        virtual float sdfSurface(const Vec &_p) const = 0;

     private:
        float   m_fGlassScatter;
        float   m_fSurfaceScatter;
        float   m_fIndexOfRefraction;
    };


    // diffuse material
    class MarchedSwirl : public MarchedGlass
    {
     public:
        MarchedSwirl(float _fGlassScatter, float _fSurfaceScatter, float _fIndexOfRefraction)
            :MarchedGlass(_fGlassScatter, _fSurfaceScatter, _fIndexOfRefraction)
        {}

     protected:
        // surface color
        virtual void colorSurface(Color &_diffuse, Color &_emitted, const Vec &_p) const override {
            _diffuse = Color(1, 0, 0) + (Color(0, 1, 0) * fabs(_p.y()/8)).clamp();
            _emitted = Color();
        }
        
        virtual float sdfSurface(const Vec &_p) const override {
            auto axis = axisEulerZYX(0, _p.y()/6, 0);
            auto pr = axis.rotateFrom(_p);
            
            return _p.size() - 5 - 8 * sin(pr.x()/2) * sin(pr.z()/2);
        }
    };
    
};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_MARCHED_MATERIALS_H
