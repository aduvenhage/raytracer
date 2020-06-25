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
                 ((pHitShape == nullptr) || (dPositionOnRay < dOnRayMin)) ) {
                pHitShape = pShape.get();
                dOnRayMin = dPositionOnRay;
            }
        }

        if ( (pHitShape != nullptr) && (dOnRayMin > 0) ) {
            auto intersect = Intersect(_ray, dOnRayMin);
            auto color = Color(1.0, 1.0, 1.0);

            auto pMaterial = pHitShape->material();
            if (pMaterial != nullptr) {
                auto uv = pHitShape->uv(intersect.m_position);
                color = pHitShape->material()->color(uv);
            
                if (_max_depth > 0) {
                    // NOTE: moves ray start away from shape by a very small amount
                    auto normal = pHitShape->normal(intersect.m_position);
                    auto rayStart = intersect.m_position + 1e-4 * normal;
                    
                    double reflection = pMaterial->reflection(uv);
                    if (reflection > 0) {
                        auto reflectedRay = Ray(rayStart, reflect(_ray.m_direction, normal));
                        auto reflectedColor = trace(reflectedRay, _shapes, _max_depth - 1);
                        
                        color = color * (1 - reflection) + reflectedColor * reflection;
                    }
                    
                    double transparency = pMaterial->transparancy(uv);
                    if (transparency > 0) {
                        auto refractedRay = Ray(rayStart, reflect(_ray.m_direction, normal));
                        auto refractedColor = trace(refractedRay, _shapes, _max_depth - 1);
                        
                        color = color * (1 - transparency) + refractedColor * transparency;
                    }

                    
                }
            }
            
            return color;
        }
        else {
            return Color(0, 0, 0);  // background color
        }
    }

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_TRACE_H

