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
#include <atomic>


namespace LNF
{
    /* Ray miss handler */
    class RayMiss
    {
     public:
        virtual ~RayMiss() = default;
        
        virtual Color color(const Ray &_ray) const = 0;
    };


    /* Intersect ray with all shapes in scene */
    Intersect hit(const Ray &_ray, const std::vector<std::shared_ptr<Shape>> &_shapes) {
        Intersect ret;
        double dOnRayMin = _ray.m_dMaxDist;
        Shape *pHitShape = nullptr;
        
        for (auto &pShape : _shapes) {
            double dPositionOnRay = pShape->intersect(_ray);
            if ( (dPositionOnRay < dOnRayMin) && (dPositionOnRay > _ray.m_dMinDist) )
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
            ret.m_bInside = ret.m_normal * _ray.m_direction > 0;
        }
        
        return ret;
    }

    std::atomic<uint64_t> uTraceCount(0);


    /* Trace ray (recursively) through scene */
    Color trace(const Ray &_ray,
                const std::vector<std::shared_ptr<Shape>> &_shapes,
                const std::shared_ptr<RayMiss> &_missHandler,
                RandomGen &_randomGen,
                int _maxTraceDepth)
    {
        uTraceCount++;
        
        // create hit
        auto intersect = hit(_ray, _shapes);
        
        if (intersect == true) {
            auto pMaterial = intersect.m_pShape->material();
            
            // create scattered, reflected, reftracted, etc. color
            auto scatteredRay = pMaterial->scatter(intersect, _ray, _randomGen);
            scatteredRay.m_ray.m_origin = scatteredRay.m_ray.position(1e-4);

            if ( (_maxTraceDepth > 0) && (scatteredRay.m_color.isBlack() == false) ) {
                return scatteredRay.m_emitted +
                       scatteredRay.m_color * trace(scatteredRay.m_ray, _shapes, _missHandler, _randomGen, _maxTraceDepth - 1);
            }
            else {
                return scatteredRay.m_emitted;
            }
        }
        else {
            return _missHandler->color(_ray);  // background color
        }
    }

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_TRACE_H

