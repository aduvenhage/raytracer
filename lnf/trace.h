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
#include <functional>


namespace LNF
{
    /*
        Ray tracing functions and stats.
        Works an a pre-constructed scene.
    */
    class RayTracer
    {
     public:
        RayTracer(const Scene *_pScene, RandomGen &_randomGen, uint16_t _uMaxTraceDepth)
            :m_pScene(_pScene),
             m_randomGen(_randomGen),
             m_uTraceLimit(_uMaxTraceDepth),
             m_uTraceDepthMax(0)
        {}
        
        Color trace(const Ray &_ray, int _iPerPixelRayIndex) {
            return traceRay(_ray, _iPerPixelRayIndex, 0);
        }
        
        int traceDepthMax() const {return m_uTraceDepthMax;}

     protected:
        /* Trace ray (recursively) through scene */
        Color traceRay(const Ray &_ray, int _iPerPixelRayIndex, int _iDepth) {
            
            // check for hits on scene
            Intersect hit;
            if (m_pScene->hit(hit, _ray) == true) {
                if (hit.m_pNode != nullptr) {
                    auto pHitNode = hit.m_pNode;
                    pHitNode->intersect(hit);
                    
                    hit.m_uIterationCount = _iDepth+1;
                    m_uTraceDepthMax = std::max(hit.m_uIterationCount, m_uTraceDepthMax);
                    
                    // create scattered, reflected, refracted, etc. ray and color
                    auto scatteredRay = pHitNode->material()->scatter(hit, m_randomGen);
                    auto tracedColor = scatteredRay.m_emitted;

                    // trace recursively and blend colors
                    if ( (hit.m_uIterationCount < m_uTraceLimit) && (scatteredRay.m_color.isBlack() == false) ) {
                        // move slightly to avoid self intersection
                        scatteredRay.m_ray.m_origin = scatteredRay.m_ray.position(1e-4);
                        
                        // transform ray back to world space
                        scatteredRay.m_ray = Ray(hit.m_axis.transformFrom(scatteredRay.m_ray.m_origin),
                                                 hit.m_axis.rotateFrom(scatteredRay.m_ray.m_direction));

                        // trace again (recursively)
                        tracedColor += scatteredRay.m_color * traceRay(scatteredRay.m_ray, _iPerPixelRayIndex, _iDepth + 1);
                    }
                    
                    // check atmosphere hit
                    float fVisibility = std::min(1.0f / (hit.m_fPositionOnRay * 0.005f), 1.0f);
                    tracedColor *= fVisibility;
                    tracedColor += (1 - fVisibility) * m_pScene->backgroundColor();

                    return tracedColor;
                }
            }

            return m_pScene->backgroundColor();
        }
        
     private:
        const Scene     *m_pScene;
        RandomGen       &m_randomGen;
        uint16_t        m_uTraceLimit;
        uint16_t        m_uTraceDepthMax;
    };
    
    
    /*
        Ray marching functions and stats.
        Low level ray marching, producing surface hits from signed distance functions.
    */
    class RayMarcher
    {
     public:
        RayMarcher(int _iMaxSamples, float _fMaxDist)
            :m_iMaxSamples(_iMaxSamples),
             m_fMaxDist(_fMaxDist)
        {}

        // get marched normal from surface function
        template <typename sdf_func>
        Vec normal(const Vec &_intersect, const sdf_func &_sdf) {
            const float e = 0.0001;
            return Vec(_sdf(_intersect + Vec{e, 0, 0}) - _sdf(_intersect - Vec{e, 0, 0}),
                       _sdf(_intersect + Vec{0, e, 0}) - _sdf(_intersect - Vec{0, e, 0}),
                       _sdf(_intersect + Vec{0, 0, e}) - _sdf(_intersect - Vec{0, 0, e})).normalized();
        }


        // ray marching on provided signed distance function
        template <typename R, typename sdf_func>
        Intersect march(R &&_ray, const sdf_func &_sdf)
        {
            Intersect hit;
            const float e = 0.000001;
            float stepScale = 1.0;
            float distance = 0;
            
            // first step
            hit.m_ray = std::forward<R>(_ray);
            float dT = _sdf(hit.m_ray.position(0));
            
            if (dT < 0) {
                hit.m_bInside = true;
                stepScale *= -1;
            }
            else {
                hit.m_bInside = false;
            }
            
            // iterate until we hit or miss
            for (int i = 0; i < m_iMaxSamples; i++) {
                // check hit or miss
                float absd = fabs(dT);
                if (absd > m_fMaxDist) {
                    hit.m_uIterationCount = (uint16_t)i;
                    break;  // missed
                }
                else if (absd <= e) {
                    hit.m_fPositionOnRay = distance + dT * stepScale;
                    hit.m_position = hit.m_ray.position(hit.m_fPositionOnRay);
                    hit.m_normal = normal(hit.m_position, _sdf);
                    hit.m_uIterationCount = (uint16_t)i;
                    break;    // hit
                }
                
                // move forward
                distance += dT * stepScale;
                
                // check for surface crossing
                auto dM = _sdf(hit.m_ray.position(distance));
                if (dT * dM < -e) {
                    stepScale *= 0.8;
                }
                
                dT = dM;
            }
            
            return hit;
        }
        
     private:
        const int         m_iMaxSamples;
        const float       m_fMaxDist;
    };


    /* raytrace for a specific view to a specific output block */
    void rayTraceImage(OutputImage *_pOutput,
                       const Viewport *_pView,
                       const Scene *_scene,
                       RandomGen &_generator,
                       int _iMaxSamplesPerPixel,
                       int _iMaxDepth,
                       float _fColorTollerance)
    {
        RayTracer tracer(_scene, _generator, _iMaxDepth);

        // create rays and trace them for all pixels in block
        for (auto j = 0; j < _pOutput->height(); j++)
        {
            unsigned char *pPixel = _pOutput->row(j);
            for (auto i = 0; i < _pOutput->width(); i++)
            {
                auto stats = ColorStat();
                
                for (int k = 0; k < _iMaxSamplesPerPixel; k++)
                {
                    auto ray = _pView->getRay(i, j, _generator, k);
                    auto color = tracer.trace(ray, k);
                    stats.push(color);
                    
                    if ( (_fColorTollerance > 0.0f) &&
                         (k >= 8 * tracer.traceDepthMax()) &&
                         (stats.standardDeviation() < _fColorTollerance) )
                    {
                        break;
                    }
                }
                                
                // write averaged color to output image
                auto color = stats.mean().clamp();
                *(pPixel++) = (int)(255 * color.red() + 0.5);
                *(pPixel++) = (int)(255 * color.green() + 0.5);
                *(pPixel++) = (int)(255 * color.blue() + 0.5);
            }
        }
    }
    
    

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_TRACE_H

