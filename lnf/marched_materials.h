
#ifndef LIBS_HEADER_MARCHED_MATERIALS_H
#define LIBS_HEADER_MARCHED_MATERIALS_H


#include "material.h"
#include "color.h"
#include "intersect.h"
#include "ray.h"


namespace LNF
{
    // diffuse material
    class MarchedMaterial : public Material
    {
     public:
     
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, RandomGen &_randomGen) const override {
            if (_hit.m_bInside == false) {
                return surfaceScatter(_hit.m_position, _hit.m_ray.m_direction, _randomGen);
            }
            else {
                return ScatteredRay(Ray(_hit.m_position, _hit.m_ray.m_direction),
                                    Color(1.0f, 1.0f, 1.0f),
                                    Color());
            }
        }
        
     protected:
        // get surface scattered ray
        ScatteredRay surfaceScatter(const Vec &_origin, const Vec &_ray, RandomGen &_randomGen) const {
            Vec p(_origin);
            bool bIntersect = getSurfaceIntersect(p, _ray);
        
            if (bIntersect == true) {
                auto normal = (getSurfaceNormal(p) + randomUnitSphere(_randomGen) * 0.1).normalized();
                auto reflectedRay = Ray(p, reflect(_ray, normal));
                
                Color diffuseColor, emittedColor;
                getSurfaceColor(diffuseColor, emittedColor, p);
                
                return ScatteredRay(reflectedRay, diffuseColor, emittedColor);
            }
            else {
                return ScatteredRay(Ray(_origin, _ray),
                                    Color(1.0f, 1.0f, 1.0f),
                                    Color());
            }
        }
        
        // get intersection
        bool getSurfaceIntersect(Vec &_p, const Vec &_ray, const float _fStepScale = 0.5) const {
            const float e = 0.00001;
            const int n = 1000;
            const float maxDist = 500;
            
            for (int i = 0; i < n; i++) {
                float distance = dfSurface(_p);
                if (distance > maxDist) {
                    return false;
                }
                else if (fabs(distance) <= e) {
                    return true;
                }
                else if (distance < e) {
                    _p = _p + _ray * distance * _fStepScale * 0.2;
                }
                else {
                    _p = _p + _ray * distance * _fStepScale;
                }
            }
            
            return false;
        }
        
        // get normal
        Vec getSurfaceNormal(const Vec &_p) const {
            const float e = 0.0001;
            return Vec(dfSurface(_p + Vec{e, 0, 0}) - dfSurface(_p - Vec{e, 0, 0}),
                       dfSurface(_p + Vec{0, e, 0}) - dfSurface(_p - Vec{0, e, 0}),
                       dfSurface(_p + Vec{0, 0, e}) - dfSurface(_p - Vec{0, 0, e})).normalized();
        }

        // surface signed distance function
        virtual float dfSurface(const Vec &_p) const = 0;
        
        // surface color
        virtual void getSurfaceColor(Color &_diffuse, Color &_emitted, const Vec &_p) const = 0;
    };


    // diffuse material
    class DiffuseMarched : public MarchedMaterial
    {
     public:
        DiffuseMarched(const Color &_color)
            :m_color(_color)
        {}
        
     protected:
        // surface signed distance function
        virtual float dfSurface(const Vec &_p) const override {
            return dfBlob(_p);
        }
        
        // surface color
        virtual void getSurfaceColor(Color &_diffuse, Color &_emitted, const Vec &_p) const override {
            _diffuse = m_color + (Color(0, 1, 0) * fabs(_p.y()/8)).clamp();
            _emitted = Color();
        }
        
     private:
        float dfSphere(const Vec &_p) const {
            return _p.size() - 10;
        }
        
        float dfBlob(const Vec &_p) const {
            auto axis = axisEulerZYX(0, _p.y()/6, 0);
            auto pr = axis.rotateFrom(_p);
            
            return _p.size() - 5 - 5 * sin(pr.x()/2) * sin(pr.z()/2);
        }
        
        float dfCube(const Vec &_p) const {
            return maxElement(_p.abs() - Vec(8, 8, 8));
        }

     private:
        Color           m_color;
    };
    
};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_MARCHED_MATERIALS_H
