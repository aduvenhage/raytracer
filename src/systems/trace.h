#ifndef SYSTEMS_TRACE_H
#define SYSTEMS_TRACE_H


#include "core/color.h"
#include "core/constants.h"
#include "core/outputimage.h"
#include "core/random.h"
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
             m_uRayCount(0)
        {}
        
        template <typename R>
        CORE::Color trace(R &&_ray) {
            const uint16_t bounceMin = 3;
            CORE::Color tracedColor(0, 0, 0);
            CORE::Color attColor(1, 1, 1);
            CORE::Ray ray(std::forward<R>(_ray));
            std::uniform_real_distribution<float> uniform01(0, 1.0);
            
            for (uint16_t i = 0; i < m_uTraceLimit; i++) {
                m_uRayCount++;
                BASE::Intersect hit(ray);

                // check for hits on scene
                if (m_pScene->hit(hit) == true) {
                    // complete hit
                    hit.m_pPrimitive->intersect(hit);
                    hit.m_uTraceDepth = i + 1;

                    // calculate hit on material
                    auto scatteredRay = hit.m_pPrimitive->material()->scatter(hit);
                    tracedColor += attColor * scatteredRay.m_emitted;
                    attColor *= scatteredRay.m_color;
                    
                    // stop on long paths
                    if (i > bounceMin) {
                        float p = attColor.max();
                        if (p < uniform01(CORE::generator())) {
                            break;  // stop -- attenuation very low
                        }
                        
                        attColor *= 1.0f/p;
                    }

                    // transform ray back to world space
                    ray = hit.m_pPrimitive->transformRayFrom(scatteredRay.m_ray);
                }
                else {
                    tracedColor += attColor * m_pScene->backgroundColor();
                    break;  // stop -- no hits
                }
            }
            
            return tracedColor;
        }
        
        uint64_t rayCount() const {return m_uRayCount;}

     private:
        const BASE::Scene   *m_pScene;
        uint16_t            m_uTraceLimit;
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
        const int maxSamples = 100000;
        const float e = 0.00001f;
        float stepScale = 1.0f/_hit.m_priRay.m_direction.size();
        
        // check inside/outside
        float dT = _sdf(_hit.m_priRay.m_origin) * stepScale;
        if (dT < 0) {
            _hit.m_bInside = true;
            stepScale *= -1;
        }
        else {
            _hit.m_bInside = false;
        }

        float distance = dT * stepScale;
        
        // iterate until we hit or miss
        bool bHit = false;
        int i = 0;

        while (i < maxSamples) {
            // check hit or miss
            if (fabs(dT) <= e) {
                bHit = true;
                break;
            }
            else if (dT > _fMaxDist) {
                break;
            }

            // check progress
            const float dM = _sdf(_hit.m_priRay.position(distance)) * stepScale;
            if (dT * dM < 0.0f) {
                stepScale *= 0.5f;
            }
            
            // continue
            distance += dM;
            dT = dM;
            i++;
        }
        
        
        if (bHit == true) {
            _hit.m_fPositionOnRay = distance;
            _hit.m_position = _hit.m_priRay.position(distance);
            _hit.m_uMarchDepth = (uint16_t)i;
            return true;
        }
        else {
            _hit.m_uMarchDepth = (uint16_t)i;
            return false;
        }
    }
    

};  // namespace SYSTEMS

#endif  // #ifndef SYSTEMS_TRACE_H

