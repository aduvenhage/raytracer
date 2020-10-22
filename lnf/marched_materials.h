
#ifndef LIBS_HEADER_MARCHED_MATERIALS_H
#define LIBS_HEADER_MARCHED_MATERIALS_H


#include "material.h"
#include "color.h"
#include "intersect.h"
#include "ray.h"


namespace LNF
{
    // surface signed distance function
    struct dfSurface {
        float operator()(const Vec &_p) const {
            auto axis = axisEulerZYX(0, _p.y()/6, 0);
            auto pr = axis.rotateFrom(_p);
            
            return _p.size() - 5 - 8 * sin(pr.x()/2) * sin(pr.z()/2);
        }
    };


    // diffuse material
    class MarchedGlass : public Material
    {
     public:
     
        /* Returns the scattered ray at the intersection point. */
        virtual ScatteredRay scatter(const Intersect &_hit, RandomGen &_randomGen) const override {
            // reflect/refract through glass
            auto ray = refract(_hit.m_ray.m_direction, _hit.m_normal, 1.5, _hit.m_bInside, 0.01, _randomGen);
            if (_hit.m_bInside == false) {
                // try to hit surface inside
                Vec pos, normal;
                bool bHit = marchedTrace(pos, normal,
                                         Ray(_hit.m_position, ray),
                                         dfSurface(), 0.2,
                                         100);
                                         
                if (bHit == true) {
                    // reflect off surface
                    auto surfaceNormal = (normal + randomUnitSphere(_randomGen) * 0.1).normalized();
                    return ScatteredRay(Ray(pos, reflect(ray, surfaceNormal)),
                                        Color(0.0f, 1.0f, 0.0f),
                                        Color());
                }
            }

            // no inner hit, just return refracted glass ray
            return ScatteredRay(Ray(_hit.m_position, ray),
                                Color(1.0f, 1.0f, 1.0f),
                                Color());
        }
        
     protected:
        // surface color
        virtual void getSurfaceColor(Color &_diffuse, Color &_emitted, const Vec &_p) const {
            _diffuse = Color(1, 0, 0) + (Color(0, 1, 0) * fabs(_p.y()/8)).clamp();
            _emitted = Color();
        }
    };

    
};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_MARCHED_MATERIALS_H
