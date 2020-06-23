#ifndef LIBS_HEADER_TRACE_H
#define LIBS_HEADER_TRACE_H


#include "color.h"
#include "constants.h"
#include "ray.h"
#include "sphere.h"
#include "uv.h"
#include "vec3.h"
#include "mandlebrot.h"

#include <vector>


namespace LNF
{
    MandleBrot mb(1, 1);

    Color trace(const Ray &_ray, const std::vector<Sphere> &_spheres, int _max_depth)
    {
        double dOnRayMin = 0;
        int si = -1;
        
        for (auto i = 0; i < _spheres.size(); i++) {
            auto &sphere = _spheres[i];
            
            double dPositionOnRay = sphere.intersect(_ray);
            if ( (dPositionOnRay > 0) &&
                 ((si < 0) || (dPositionOnRay < dOnRayMin)) ) {
                si = i;
                dOnRayMin = dPositionOnRay;
            }
        }

        if ( (si >= 0) && (dOnRayMin > 0) ) {
            auto &sphere = _spheres[si];
            auto intersect = Intersect(_ray, dOnRayMin);
            auto uv = sphere.uv(intersect.m_position);
            auto normal = sphere.normal(intersect.m_position);
            auto color = sphere.m_color;
            
            if (si == 0) {
                color *= (((int)(uv.m_dU * 16) + (int)(uv.m_dV * 16)) % 2) * 0.5 + 0.5;
            }
            else {
                color *= ((Color(0.1, 0.2, 0.3) * mb.value(uv.m_dU, uv.m_dV)).wrap() + Color(0.1, 0.1, 0.1)).clamp();
            }
            
            if (_max_depth > 0) {
                
                if (sphere.m_dReflection > 0) {
                    auto reflectedRay = Ray(intersect.m_position + 1e-4 * normal, reflect(_ray.m_direction, normal));
                    auto reflectedColor = trace(reflectedRay, _spheres, _max_depth - 1);
                    
                    color = color * (1 - sphere.m_dReflection) + reflectedColor * sphere.m_dReflection;
                }
            }
            
            return color;
        }
        else {
            return Color();
        }
    }

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_TRACE_H

