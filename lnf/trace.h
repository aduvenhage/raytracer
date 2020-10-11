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
    /* Trace ray (recursively) through scene */
    Color trace(const Ray &_ray,
                const Scene *_pScene,
                RandomGen &_randomGen,
                int _iMaxTraceDepth)
    {
        Intersect hit;
        if (_pScene->hit(hit, _ray) == true) {
            if (hit.m_pNode != nullptr) {
                // complete intercept
                auto pHitNode = hit.m_pNode;
                pHitNode->intersect(hit);
            
                // create scattered, reflected, reftracted, etc. color
                auto pMaterial = pHitNode->material();
                auto scatteredRay = pMaterial->scatter(hit, _randomGen);
                scatteredRay.m_ray.m_origin = scatteredRay.m_ray.position(1e-4);
                
                // transform ray back to world space
                scatteredRay.m_ray.m_direction = hit.m_axis.rotateFrom(scatteredRay.m_ray.m_direction);
                scatteredRay.m_ray.m_origin = hit.m_axis.transformFrom(scatteredRay.m_ray.m_origin);

                // trace recursively and blend colors
                if ( (_iMaxTraceDepth > 0) && (scatteredRay.m_color.isBlack() == false) ) {
                    auto tracedColor = trace(scatteredRay.m_ray, _pScene, _randomGen, _iMaxTraceDepth - 1);
                    return scatteredRay.m_emitted + scatteredRay.m_color * tracedColor;
                }
                else {
                    return scatteredRay.m_emitted;
                }
            }
        }
        
        return _pScene->missColor(_ray);  // background color
    }


    /* raytrace for a specific view to a specific output block */
    void renderImage(OutputImage *_pOutput,
                     const Viewport *_pView,
                     const Scene *_scene,
                     RandomGen &_generator,
                     int _iRaysPerPixel, int _iMaxDepth)
    {
        // create rays and trace them for all pixels in block
        for (auto j = 0; j < _pOutput->height(); j++)
        {
            unsigned char *pPixel = _pOutput->row(j);
            for (auto i = 0; i < _pOutput->width(); i++)
            {
                auto color = Color();
                for (int k = 0; k < _iRaysPerPixel; k++)
                {
                    auto ray = _pView->getRay(i, j, _generator);
                    color += trace(ray, _scene, _generator, _iMaxDepth - 1);
                }
                                
                // write averaged color to output image
                color = (color / _iRaysPerPixel).clamp();
                *(pPixel++) = (int)(255 * color.red() + 0.5);
                *(pPixel++) = (int)(255 * color.green() + 0.5);
                *(pPixel++) = (int)(255 * color.blue() + 0.5);
            }
        }
    }

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_TRACE_H

