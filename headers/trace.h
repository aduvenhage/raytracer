#ifndef LIBS_HEADER_TRACE_H
#define LIBS_HEADER_TRACE_H


#include "color.h"
#include "constants.h"
#include "intersect.h"
#include "material.h"
#include "ray.h"
#include "sphere.h"
#include "uv.h"
#include "vec3.h"

#include <vector>
#include <random>


namespace LNF
{
    Intersect hit(const Ray &_ray, const std::vector<std::shared_ptr<Shape>> &_shapes) {
        Intersect ret;
        double dOnRayMin = 0;
        Shape *pHitShape = nullptr;
        
        for (auto &pShape : _shapes) {
            double dPositionOnRay = pShape->intersect(_ray);
            if ( (dPositionOnRay > 0.00000001) &&
                 ((pHitShape == nullptr) || (dPositionOnRay < dOnRayMin)) )
            {
                pHitShape = pShape.get();
                dOnRayMin = dPositionOnRay;
            }
        }
        
        if ( (pHitShape != nullptr) &&
             (dOnRayMin > 0) )
        {
            ret = Intersect(pHitShape, _ray, dOnRayMin);
            ret.m_normal = pHitShape->normal(ret.m_position);
            ret.m_uv = pHitShape->uv(ret.m_position);
        }
        
        return ret;
    }


    Color trace(const Ray &_ray,
                const std::vector<std::shared_ptr<Shape>> &_shapes,
                const std::default_random_engine &_randomGen,
                int _max_depth)
    {
        // create hit
        if (_max_depth > 0) {
            auto intersect = hit(_ray, _shapes);
            if (intersect == true) {
                // trace through shape
                auto pMaterial = intersect.m_pShape->material();
                auto scatteredRay = pMaterial->scatter(intersect, _ray, _randomGen);
                
                scatteredRay.m_ray.m_origin = scatteredRay.m_ray.position(1e-4);  // move slighly to avoid self collision
                
                return scatteredRay.m_color * trace(scatteredRay.m_ray, _shapes, _randomGen, _max_depth - 1);
            }
        }
        
        return Color(0.8, 0.8, 0.8);  // background color
    }
            
            /*
            auto color = pHitShape->color(uv);
            double kr = pHitShape->reflection();

            auto reflectedColor = Color();
            
            if (kr > 0.0) {
                if (kr < 1.0) {
                    normal = (normal + randomCubeVec() * (1 - kr) * 0.4).normalized();
                }
                
                auto reflectedRay = Ray(intersect.m_position, reflect(_ray.m_direction, normal));
                reflectedColor = trace(reflectedRay, _shapes, _max_depth - 1);
            }
            
            return (color * (1.0 - kr) + reflectedColor * kr).clamp();
            */
            
            /*
            if ( (kt > 0.00001) && (kr > 0.00001) ) {
                kr *= fresnel(_ray.m_direction, normal, pHitShape->indexOfRefraction());
                kt *= (1.0 - kr);
            }
            
            auto reflectedColor = Color();
            auto refractedColor = Color();

            if (kr > 0.00001) {
                auto reflectedRay = Ray(intersect.m_position, reflect(_ray.m_direction, normal));
                reflectedRay.m_origin = reflectedRay.position(1e-4);  // move slighly to avoid self collision
                
                reflectedColor = trace(reflectedRay, _shapes, _max_depth - 1) * kr;
            }
            
            if (kt > 0.00001) {
                auto refractedRay = Ray(intersect.m_position, refract(_ray.m_direction, normal, pHitShape->indexOfRefraction()));
                refractedRay.m_origin = refractedRay.position(1e-4);  // move slighly to avoid self collision
                
                refractedColor = color * trace(refractedRay, _shapes, _max_depth - 1) * kt;
            }
            
            return (color * (1 - kt) * (1 - kr) + refractedColor + reflectedColor).clamp();
            */

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_TRACE_H

