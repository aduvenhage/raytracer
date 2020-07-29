#ifndef LIBS_HEADER_TRACE_H
#define LIBS_HEADER_TRACE_H


#include "color.h"
#include "constants.h"
#include "intersect.h"
#include "material.h"
#include "outputimage.h"
#include "ray.h"
#include "scene.h"
#include "uv.h"
#include "vec3.h"
#include "viewport.h"

#include <vector>
#include <random>
#include <atomic>


namespace LNF
{
    std::atomic<uint64_t> uTraceCount(0);

    /* Trace ray (recursively) through scene */
    Color trace(const Ray &_ray,
                const std::shared_ptr<Scene> &_pScene,
                RandomGen &_randomGen,
                int _maxTraceDepth)
    {
        uTraceCount++;
        auto intersect = _pScene->hit(_ray);
        if (intersect == true) {
            auto pHitShape = intersect.m_pShape;
            
            // complete intersect info
            intersect.m_normal = pHitShape->normal(intersect.m_position);
            intersect.m_uv = pHitShape->uv(intersect.m_position);
            intersect.m_bInside = intersect.m_normal * _ray.m_direction > 0;
                        
            // create scattered, reflected, reftracted, etc. color
            auto pMaterial = pHitShape->material();
            auto scatteredRay = pMaterial->scatter(intersect, _ray, _randomGen);
            scatteredRay.m_ray.m_origin = scatteredRay.m_ray.position(1e-4);

            if ( (_maxTraceDepth > 0) && (scatteredRay.m_color.isBlack() == false) ) {
                return scatteredRay.m_emitted +
                       scatteredRay.m_color * trace(scatteredRay.m_ray, _pScene, _randomGen, _maxTraceDepth - 1);
            }
            else {
                return scatteredRay.m_emitted;
            }
        }
        else {
            return _pScene->missColor(_ray);  // background color
        }
    }


    /* raytrace for a specific view to a specific output block */
    void renderImage(const std::unique_ptr<OutputImage> &_pOutput,
                     const std::unique_ptr<Viewport> &_pView,
                     const std::shared_ptr<Scene> &_scene,
                     RandomGen &_generator,
                     int _iRaysPerPixel, int _iMaxDepth)
    {
        std::uniform_real_distribution<double> pixelDist(-0.4, 0.4);
        
        // create rays and trace them for all pixels in block
        for (auto j = 0; j < _pOutput->height(); j++)
        {
            unsigned char *pPixel = _pOutput->row(j);
            for (auto i = 0; i < _pOutput->width(); i++)
            {
                auto color = Color();
                
                for (int k = 0; k < _iRaysPerPixel; k++)
                {
                    // get ray with some fuzziness around pixel center
                    auto ray = _pView->getRay(i, j, pixelDist(_generator), pixelDist(_generator));

                    // trace ray and add color result
                    color += LNF::trace(ray, _scene, _generator, _iMaxDepth);
                }
                                
                // write averaged color to output image
                color = (color / _iRaysPerPixel).clamp();
                *(pPixel++) = (int)(255 * color.m_fRed);
                *(pPixel++) = (int)(255 * color.m_fGreen);
                *(pPixel++) = (int)(255 * color.m_fBlue);
            }
        }
    }

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_TRACE_H

