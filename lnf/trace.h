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
            hit.m_viewRay = _ray;
            
            if (m_pScene->hit(hit, m_randomGen) == true) {
                // update stats
                hit.m_uTraceDepth = _iDepth+1;
                m_uTraceDepthMax = std::max(hit.m_uTraceDepth, m_uTraceDepthMax);
                
                // normal hit -- complete hit
                hit.m_pPrimitive->intersect(hit);
                
                // create scattered, reflected, refracted, etc. ray and color
                auto scatteredRay = hit.m_pPrimitive->material()->scatter(hit, m_randomGen);
                auto tracedColor = scatteredRay.m_emitted;

                // trace recursively and blend colors
                if ( (hit.m_uTraceDepth < m_uTraceLimit) && (scatteredRay.m_color.isBlack() == false) ) {
                    // move slightly to avoid self intersection
                    scatteredRay.m_ray.m_origin = scatteredRay.m_ray.position(1e-4);
                    
                    // transform ray back to world space
                    scatteredRay.m_ray = hit.m_pPrimitive->transformRayFrom(scatteredRay.m_ray);

                    // trace again (recursively)
                    tracedColor += scatteredRay.m_color * traceRay(scatteredRay.m_ray, _iPerPixelRayIndex, _iDepth + 1);
                }

                return tracedColor;
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
     Ray marching on provided signed distance function.
     Attributes populated in _hit:
        - m_position
        - m_bInside
        - m_uHitIterationCount
        - m_fPositionOnRay
     
     */
    template <typename sdf_func>
    bool check_marched_hit(Intersect &_hit, float _fMaxDist, const sdf_func &_sdf)
    {
        // TODO: play with these values and maybe adjust dynamically for best performance
        // TODO: exit criteria could include min stepScale
        const int maxSamples = 100000;
        const float e = 0.00001;
        float stepScale = 1.0;
        float distance = 0;
        
        // first step (check inside/outside)
        _hit.m_position = _hit.m_priRay.position(0);
        float dT = _sdf(_hit.m_position);
        
        if (dT < 0) {
            _hit.m_bInside = true;
            stepScale *= -1;
        }
        else {
            _hit.m_bInside = false;
        }
        
        // iterate until we hit or miss
        for (int i = 0; i < maxSamples; i++) {
            // check hit or miss
            float absd = fabs(dT);
            if (absd > _fMaxDist) {
                _hit.m_uMarchDepth = (uint16_t)i;
                return false;  // missed
            }
            else if (absd <= e) {
                _hit.m_fPositionOnRay = distance + dT * stepScale;
                _hit.m_uMarchDepth = (uint16_t)i;
                return true;    // hit
            }
            
            // move forward
            distance += dT * stepScale;
            
            // check for surface crossing
            _hit.m_position = _hit.m_priRay.position(distance);
            auto dM = _sdf(_hit.m_position);
            if (dT * dM < -e) {
                stepScale *= 0.5;
            }
            
            dT = dM;
        }
        
        return false;       // missed
    }


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
                         (k >= 4 * tracer.traceDepthMax() + 8) &&
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

