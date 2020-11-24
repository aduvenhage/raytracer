
#ifndef LIBS_HEADER_MARCHED_MATERIALS_H
#define LIBS_HEADER_MARCHED_MATERIALS_H


#include "material.h"
#include "color.h"
#include "intersect.h"
#include "ray.h"


namespace LNF
{
    // ray marched material base class
    class MarchedMaterial : public Material
    {
     public:
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, RandomGen &_randomGen) const override {
            RayMarcher marcher;
            
            // transform ray on shape boundary (raytraced hit)
            auto ray = getShapeScatter(_hit.m_ray.m_direction, _hit.m_normal, _hit.m_bInside, _randomGen);
            
            // try to hit surface inside (using raymarching)
            if (ray * _hit.m_normal < 0) {
                auto hit = marcher.march(Ray(_hit.m_position, ray),
                                         [this](const Vec &_p){return this->sdfSurface(_p);});
                                         
                if (hit == true) {
                    auto raym = getSurfaceScatter(hit.m_ray.m_direction, hit.m_normal, hit.m_bInside, _randomGen);
                    return color(raym, hit.m_position, hit.m_normal, true);
                }
            }

            // no inner hit, just return scattered ray
            return color(ray, _hit.m_position, _hit.m_normal, false);
        }
        
     protected:
        // bounding shape scatter (raytraced hit)
        virtual Vec getShapeScatter(const Vec &_ray, const Vec &_normal, bool _bInside, RandomGen &_randomGen) const = 0;
        
        // inner surface scatter (raymarched hit)
        virtual Vec getSurfaceScatter(const Vec &_ray, const Vec &_normal, bool _bInside, RandomGen &_randomGen) const = 0;
        
        // surface signed distance function
        virtual float sdfSurface(const Vec &_p) const = 0;
        
        // calc surface color
        virtual ScatteredRay color(const Vec &_ray, const Vec &_pos, const Vec &_normal, bool _bSurfaceHit) const = 0;
    };


    // glass material with ray marched surface inside
    class MarchedGlass : public MarchedMaterial
    {
     public:
        MarchedGlass(float _fGlassScatter, float _fSurfaceScatter, float _fIndexOfRefraction, float _fOpacity)
            :m_fGlassScatter(_fGlassScatter),
             m_fSurfaceScatter(_fSurfaceScatter),
             m_fIndexOfRefraction(_fIndexOfRefraction),
             m_fOpacity(_fOpacity)
        {}

     protected:
        // bounding shape scatter (raytraced hit)
        virtual Vec getShapeScatter(const Vec &_ray, const Vec &_normal, bool _bInside, RandomGen &_randomGen) const override {
            return refract(_ray, _normal, m_fIndexOfRefraction, _bInside, m_fGlassScatter, _randomGen);
        }
        
        // inner surface scatter (raymarched hit)
        virtual Vec getSurfaceScatter(const Vec &_ray, const Vec &_normal, bool _bInside, RandomGen &_randomGen) const override {
            std::uniform_real_distribution<float> uniform01(0, 1);
            if (uniform01(_randomGen) < m_fOpacity) {
                auto surfaceNormal = (_normal + randomUnitSphere(_randomGen) * m_fSurfaceScatter).normalized();
                return reflect(_ray, surfaceNormal);
            }
            else {
                return _ray;
            }
        }

     private:
        float       m_fGlassScatter;
        float       m_fSurfaceScatter;
        float       m_fIndexOfRefraction;
        float       m_fOpacity;
    };


    // marched material in air
    class MarchedSurface : public MarchedMaterial
    {
     public:
        MarchedSurface(float _fSurfaceScatter, float _fIndexOfRefraction)
            :m_fSurfaceScatter(_fSurfaceScatter),
             m_fIndexOfRefraction(_fIndexOfRefraction)
        {}

     protected:
        // bounding shape scatter (raytraced hit)
        virtual Vec getShapeScatter(const Vec &_ray, const Vec &, bool , RandomGen &) const override {
            return _ray;
        }
        
        // inner surface scatter (raymarched hit)
        virtual Vec getSurfaceScatter(const Vec &_ray, const Vec &_normal, bool _bInside, RandomGen &_randomGen) const override {
            return refract(_ray, _normal, m_fIndexOfRefraction, _bInside, m_fSurfaceScatter, _randomGen);
        }

     private:
        float       m_fSurfaceScatter;
        float       m_fIndexOfRefraction;
    };



    // coloured swirls in glass
    class GlassSwirl : public MarchedGlass
    {
     public:
        GlassSwirl(float _fGlassScatter, float _fSurfaceScatter, float _fIndexOfRefraction)
            :MarchedGlass(_fGlassScatter, _fSurfaceScatter, _fIndexOfRefraction, 1.0)
        {}

     protected:
        // calc surface color
        virtual ScatteredRay color(const Vec &_ray, const Vec &_pos, const Vec &_normal, bool _bSurfaceHit) const override {
            if (_bSurfaceHit == true) {
                return ScatteredRay(
                    Ray(_pos, _ray),
                    Color((_normal.x() + 1)/2, (_normal.y() + 1)/2, (_normal.z() + 1)/2),
                    Color()
                );
            }
            else {
                return ScatteredRay(
                    Ray(_pos, _ray),
                    Color(1.0f, 1.0f, 1.0f),
                    Color()
                );
            }
        }
        
        // surface signed distance function
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
        // calc surface color
        virtual ScatteredRay color(const Vec &_ray, const Vec &_pos, const Vec &_normal, bool _bSurfaceHit) const override {
            if (_bSurfaceHit == true) {
                float sd1 = sdfBubbles(_pos, 0);
                float sd2 = sdfBubbles(_pos, M_PI);
                if (sd1 < sd2) {
                    return ScatteredRay(
                        Ray(_pos, _ray),
                        Color((_normal.x() + 1)/2, (_normal.y() + 1)/2, (_normal.z() + 1)/2),
                        Color()
                    );
                }
                else {
                    return ScatteredRay(
                        Ray(_pos, _ray),
                        Color(0.1, 1, 0.1),
                        Color()
                    );
                }
            }
            else {
                return ScatteredRay(
                    Ray(_pos, _ray),
                    Color(1.0f, 1.0f, 1.0f),
                    Color()
                );
            }
        }
        
        // surface signed distance function
        virtual float sdfSurface(const Vec &_p) const override {
            const float scale = 0.2;
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
                Vec origin(2*sin(t * M_PI * 4 + _dAngleY), (t - 0.5) * 30, 1.5*cos(t * M_PI * 4 + _dAngleY));
                sdf += exp(-k * dfSphere(_p, origin,  frac(t/0.3) * 1 + 0.1));
            }
            
            return -log(sdf);
        }
    };
    


    // coloured swirls in glass
    class Swirl : public MarchedSurface
    {
     public:
        Swirl(float _fSurfaceScatter, float _fIndexOfRefraction)
            :MarchedSurface(_fSurfaceScatter, _fIndexOfRefraction)
        {}

     protected:
        // calc surface color
        virtual ScatteredRay color(const Vec &_ray, const Vec &_pos, const Vec &, bool) const override {
            return ScatteredRay(
                Ray(_pos, _ray),
                Color(1.0f, 1.0f, 1.0f),
                Color()
            );
        }

        // surface signed distance function
        virtual float sdfSurface(const Vec &_p) const override {
            const float scale = 0.5;
            auto axis = axisEulerZYX(0, _p.y()/6, 0);
            auto pr = axis.rotateFrom(_p);
            
            return (_p.size() - 5 - 8 * sin(pr.x()/2) * sin(pr.z()/2)) * scale;
        }
    };


    // coloured swirls in glass
    class MarchedSphere : public MarchedSurface
    {
     public:
        MarchedSphere(float _fRadius, float _fSurfaceScatter, float _fIndexOfRefraction)
            :MarchedSurface(_fSurfaceScatter, _fIndexOfRefraction),
             m_fRadius(_fRadius)
        {}

     protected:
        // calc surface color
        virtual ScatteredRay color(const Vec &_ray, const Vec &_pos, const Vec &, bool _bSurfaceHit) const override {
            if (_bSurfaceHit == false) {
                return ScatteredRay(
                    Ray(_pos, _ray),
                    Color(1.0f, 1.0f, 1.0f),
                    Color()
                );
            }
            else {
                return ScatteredRay(
                    Ray(_pos, _ray),
                    Color(0.9f, 0.9f, 0.9f),
                    Color()
                );
            }
        }

        // surface signed distance function
        virtual float sdfSurface(const Vec &_p) const override {
            return _p.size() - m_fRadius - 3 * sin(_p.x()/2) * sin(_p.y()/2) * sin(_p.z()/2);
        }
        
     private:
        float   m_fRadius;
    };


};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_MARCHED_MATERIALS_H
