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

    Color trace(const Ray &_ray, const std::vector<Sphere> &_spheres, int count = 0)
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
            
            double shade = fabs(_ray.m_direction.inverse() * normal * 0.2);
            
            if (si == 0) {
                shade += (((int)(uv.m_dU * 24) + (int)(uv.m_dV * 24)) % 2) * 0.8;
            }
            else {
                shade += (double)mb.value(uv.m_dU, uv.m_dV) / (double)mb.max_iterations() * 0.8;
            }
            
            color = color * shade;
            
            // Vec3f refldir = raydir - nhit * 2 * raydir.dot(nhit);
            if (count < 10) {
                auto reflectedRay = Ray(intersect.m_position, normal);
                auto reflectedColor = trace(reflectedRay, _spheres, count + 1);
                
                color = color * 0.4 + reflectedColor * 0.8;
            }
            
            return color;
        }
        else {
            return Color();
        }
    }

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_TRACE_H

