#ifndef LIBS_HEADER_TRACE_H
#define LIBS_HEADER_TRACE_H


#include "color.h"
#include "constants.h"
#include "ray.h"
#include "sphere.h"
#include "uv.h"
#include "vec3.h"

#include <vector>


namespace LNF
{
    MandleBrot mb(1, 1);

    Color trace(const Ray &_ray, const std::vector<std::shared_ptr<Shape>> &_shapes, int _max_depth)
    {
        double dOnRayMin = 0;
        Shape *pHitShape = nullptr;
        
        for (auto &pShape : _shapes) {
            double dPositionOnRay = pShape->intersect(_ray);
            if ( (dPositionOnRay > 0) &&
                 ((pHitShape == nullptr) || (dPositionOnRay < dOnRayMin)) )
            {
                pHitShape = pShape.get();
                dOnRayMin = dPositionOnRay;
            }
        }

        if ( (pHitShape != nullptr) &&
             (dOnRayMin > 0) &&
             (_max_depth > 0) )
        {
            auto pMaterial = pHitShape->material();
            if (pMaterial != nullptr) {
                auto intersect = Intersect(_ray, dOnRayMin);
                auto normal = pHitShape->normal(intersect.m_position);
                auto uv = pHitShape->uv(intersect.m_position);
                auto color = pHitShape->material()->color(uv);
                double kt = pMaterial->transparancy(uv);
                double kr = pMaterial->reflection(uv);
                
                if ( (kt > 0.00001) && (kr > 0.00001) ) {
                    kr *= fresnel(_ray.m_direction, normal, pMaterial->indexOfRefraction());
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
                    auto refractedRay = Ray(intersect.m_position, refract(_ray.m_direction, normal, pMaterial->indexOfRefraction()));
                    refractedRay.m_origin = refractedRay.position(1e-4);  // move slighly to avoid self collision
                    
                    refractedColor = color * trace(refractedRay, _shapes, _max_depth - 1) * kt;
                }
                
                return (color * (1 - kt) * (1 - kr) + refractedColor + reflectedColor).clamp();
            }
            else {
                return Color(1.0, 1.0, 1.0);
            }
        }
        
        return Color(0, 0, 0);  // background color
    }

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_TRACE_H

