
#ifndef LIBS_HEADER_MARCHED_MATERIALS_H
#define LIBS_HEADER_MARCHED_MATERIALS_H


#include "material.h"
#include "color.h"
#include "intersect.h"
#include "ray.h"


namespace LNF
{
    // glass material with ray marched surface inside (base class)
    class MarchedGlass : public Material
    {
     public:
        MarchedGlass(float _fGlassScatter, float _fSurfaceScatter, float _fIndexOfRefraction, float _fOpacity)
            :m_fGlassScatter(_fGlassScatter),
             m_fSurfaceScatter(_fSurfaceScatter),
             m_fIndexOfRefraction(_fIndexOfRefraction),
             m_fOpacity(_fOpacity)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, RandomGen &_randomGen) const override {
            // reflect/refract through glass
            auto ray = refract(_hit.m_ray.m_direction, _hit.m_normal,
                               m_fIndexOfRefraction, _hit.m_bInside, m_fGlassScatter, _randomGen);

            // try to hit surface inside (using signed distance function)
            Vec pos, normal;
            bool inside;
            bool bHit = marchedTrace(pos, normal, inside,
                                     Ray(_hit.m_position, ray),
                                     [this](const Vec &_p){return this->sdfSurface(_p);});
                                     
            if (bHit == true) {
                std::uniform_real_distribution<float> uniform01(0, 1);
                if (uniform01(_randomGen) < m_fOpacity) {
                    // get inside surface color
                    Color emittedColor;
                    Color diffuseColor;
                    colorSurface(diffuseColor, emittedColor, pos, normal);
                    
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
        virtual void colorSurface(Color &_diffuse, Color &_emitted, const Vec &_surfacePos, const Vec &_surfaceNormal) const = 0;
        
        // surface signed distance function
        virtual float sdfSurface(const Vec &_p) const = 0;

     private:
        float       m_fGlassScatter;
        float       m_fSurfaceScatter;
        float       m_fIndexOfRefraction;
        float       m_fOpacity;
    };


    // coloured swirls in glass
    class GlassSwirl : public MarchedGlass
    {
     public:
        GlassSwirl(float _fGlassScatter, float _fSurfaceScatter, float _fIndexOfRefraction)
            :MarchedGlass(_fGlassScatter, _fSurfaceScatter, _fIndexOfRefraction, 1.0)
        {}

     protected:
        // surface color
        virtual void colorSurface(Color &_diffuse, Color &_emitted, const Vec &_surfacePos, const Vec &_surfaceNormal) const override {
            _diffuse = Color((_surfaceNormal.x() + 1)/2, (_surfaceNormal.y() + 1)/2, (_surfaceNormal.z() + 1)/2);
            _emitted = Color();
        }
        
        virtual float sdfSurface(const Vec &_p) const override {
            const float scale = 0.1;
            auto axis = axisEulerZYX(0, _p.y()/6, 0);
            auto pr = axis.rotateFrom(_p);
            
            return (_p.size() - 5 - 8 * sin(pr.x()/2) * sin(pr.z()/2)) * scale;
        }
    };


    // coloured bubbles in glass
    class GlassBubbles : public MarchedGlass
    {
     public:
        GlassBubbles(float _fGlassScatter, float _fSurfaceScatter, float _fIndexOfRefraction)
            :MarchedGlass(_fGlassScatter, _fSurfaceScatter, _fIndexOfRefraction, 0.8)
        {}

     protected:
        // surface color
        virtual void colorSurface(Color &_diffuse, Color &_emitted, const Vec &_surfacePos, const Vec &_surfaceNormal) const override {
            float sd1 = sdfBubbles(_surfacePos, 0);
            float sd2 = sdfBubbles(_surfacePos, M_PI);
            if (sd1 < sd2) {
                _diffuse = Color((_surfaceNormal.x() + 1)/2, (_surfaceNormal.y() + 1)/2, (_surfaceNormal.z() + 1)/2);
            }
            else {
                _diffuse = Color(0.1, 1, 0.1);
            }

            _emitted = Color();
        }
        
        virtual float sdfSurface(const Vec &_p) const override {
            const float scale = 0.1;
            return std::min(sdfBubbles(_p, 0) * scale, sdfBubbles(_p, M_PI) * scale);
        }
        
     private:
        float dfSphere(const Vec &_p, const Vec &_origin, float _fRadius) const {
            return (_p - _origin).size() - _fRadius;
        }
        
        float sdfBubbles(const Vec &_p, double _dAngleY) const {
            float sdf = 0;
            float k = 3;
            std::normal_distribution<float> p(0, 1);
            
            int n = 16;
            for (int i = 0; i < n; i ++) {
                float t = (float)i/n;
                Vec origin(2*sin(t * M_PI * 4 + _dAngleY), (t - 0.5) * 20, 1.5*cos(t * M_PI * 4 + _dAngleY));
                sdf += exp(-k * dfSphere(_p, origin,  frac(t/0.3) * 1 + 0.1));
            }
            
            return -log(sdf);
        }
    };
    


    // marched material in air (base class)
    class MarchedSurface : public Material
    {
     public:
        MarchedSurface(float _fSurfaceScatter, float _fIndexOfRefraction)
            :m_fSurfaceScatter(_fSurfaceScatter),
             m_fIndexOfRefraction(_fIndexOfRefraction)
        {}
        
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, RandomGen &_randomGen) const override {
            // try to hit surface (using signed distance function)
            Vec pos, normal;
            bool inside;
            bool bHit = marchedTrace(pos, normal, inside,
                                     _hit.m_ray,
                                     [this](const Vec &_p){return this->sdfSurface(_p);});
                                     
            if (bHit == true) {
                // get inside surface color
                Color emittedColor;
                Color diffuseColor;
                colorSurface(diffuseColor, emittedColor, pos, normal);

                // refract through material
                auto ray = refract(_hit.m_ray.m_direction, normal,
                                   m_fIndexOfRefraction, inside, m_fSurfaceScatter, _randomGen);

                return ScatteredRay(Ray(pos, ray),
                                    diffuseColor,
                                    emittedColor);
            }

            // no hit, just return original ray
            return ScatteredRay(Ray(_hit.m_position, _hit.m_ray.m_direction),
                                Color(1.0f, 1.0f, 1.0f),
                                Color());
        }
        
     protected:
        // calc surface color
        virtual void colorSurface(Color &_diffuse, Color &_emitted, const Vec &_surfacePos, const Vec &_surfaceNormal) const = 0;
        
        // surface signed distance function
        virtual float sdfSurface(const Vec &_p) const = 0;

     private:
        float       m_fSurfaceScatter;
        float       m_fIndexOfRefraction;
    };
    
    
    // coloured swirls in glass
    class Swirl : public MarchedSurface
    {
     public:
        Swirl(float _fSurfaceScatter, float _fIndexOfRefraction)
            :MarchedSurface(_fSurfaceScatter, _fIndexOfRefraction)
        {}

     protected:
        // surface color
        virtual void colorSurface(Color &_diffuse, Color &_emitted, const Vec &_surfacePos, const Vec &_surfaceNormal) const override {
            _diffuse = Color(0.9, 0.9, 0.9);//Color((_surfaceNormal.x() + 1)/2, (_surfaceNormal.y() + 1)/2, (_surfaceNormal.z() + 1)/2);
            _emitted = Color();
        }
        
        virtual float sdfSurface(const Vec &_p) const override {
            const float scale = 0.1;
            auto axis = axisEulerZYX(0, _p.y()/6, 0);
            auto pr = axis.rotateFrom(_p);
            
            return (_p.size() - 5 - 8 * sin(pr.x()/2) * sin(pr.z()/2)) * scale;
        }
    };


};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_MARCHED_MATERIALS_H
