#ifndef SYSTEMS_TRACE_H
#define SYSTEMS_TRACE_H


#include "core/color.h"
#include "core/constants.h"
#include "core/outputimage.h"
#include "core/ray.h"
#include "core/uv.h"
#include "core/vec3.h"
#include "core/viewport.h"
#include "base/intersect.h"
#include "base/material.h"
#include "base/scene.h"

#include <vector>
#include <random>
#include <atomic>
#include <functional>


namespace SYSTEMS
{
    // constants
    const float T_MIN = 1e-4f;


    /*
        Ray tracing functions and stats.
        Works on a pre-constructed scene.
    */
    class RayTracer
    {
     public:
        RayTracer(const BASE::Scene *_pScene, uint16_t _uMaxTraceDepth)
            :m_pScene(_pScene),
             m_uTraceLimit(_uMaxTraceDepth),
             m_uTraceDepthMax(0),
             m_uRayCount(0)
        {}
        
        CORE::Color trace(const CORE::Ray &_ray) {
            return traceRay(_ray, 0);
        }
        
        uint16_t traceDepthMax() const {return m_uTraceDepthMax;}
        uint64_t rayCount() const {return m_uRayCount;}

     protected:
        /* Trace ray (recursively) through scene */
        CORE::Color traceRay(const CORE::Ray &_ray, int _iDepth) {
            m_uRayCount++;
            
            // check for hits on scene
            BASE::Intersect hit(_ray);
            if (m_pScene->hit(hit) == true) {
                // update stats
                hit.m_uTraceDepth = _iDepth+1;
                m_uTraceDepthMax = std::max(hit.m_uTraceDepth, m_uTraceDepthMax);
                
                // normal hit -- complete hit
                hit.m_pPrimitive->intersect(hit);
                
                // create scattered, reflected, refracted, etc. ray and color
                auto scatteredRay = hit.m_pPrimitive->material()->scatter(hit);
                auto tracedColor = scatteredRay.m_emitted;

                // trace recursively and blend colors
                if ( (hit.m_uTraceDepth < m_uTraceLimit) && (scatteredRay.m_color.isBlack() == false) ) {
                    // move slightly to avoid self intersection
                    scatteredRay.m_ray.m_origin = scatteredRay.m_ray.position(T_MIN);
                    
                    // transform ray back to world space
                    scatteredRay.m_ray = hit.m_pPrimitive->transformRayFrom(scatteredRay.m_ray);

                    // recursively trace again
                    tracedColor += scatteredRay.m_color * traceRay(scatteredRay.m_ray, _iDepth + 1);
                }

                return tracedColor;
            }

            return m_pScene->backgroundColor();
        }
        
     private:
        const BASE::Scene   *m_pScene;
        uint16_t            m_uTraceLimit;
        uint16_t            m_uTraceDepthMax;
        uint64_t            m_uRayCount;
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
    bool check_marched_hit(BASE::Intersect &_hit, float _fMaxDist, const sdf_func &_sdf)
    {
        // TODO: play with these values and maybe adjust dynamically for best performance
        // TODO: exit criteria could include min stepScale
        const int maxSamples = 100000;
        const float e = 0.00001f;
        float stepScale = 1.0f;
        float distance = 0.0f;
        
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
                stepScale *= 0.8;
            }
            
            dT = dM;
        }
        
        return false;       // missed
    }
    

};  // namespace SYSTEMS

#endif  // #ifndef SYSTEMS_TRACE_H

