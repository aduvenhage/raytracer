#pragma once

#include "core/constants.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "core/uv.h"

#include <algorithm>


namespace BASE
{
    class PrimitiveInstance;

    /*
     Container for intersect attributes and products.
     */
    struct Intersect
    {
        Intersect() noexcept = default;

        Intersect(const CORE::Ray &_viewRay) noexcept
            :m_viewRay(_viewRay)
        {}

        operator bool () const {
            return m_fPositionOnRay >= 0;
        }

        // fields populated by tracer/caller
        CORE::Ray               m_viewRay;                          // view ray
        CORE::Ray               m_priRay;                           // ray transformed for intersection with specific primitive
        const BASE::PrimitiveInstance *m_pPrimitive = nullptr;      // primitive we intersected with
        
        // key fields that should be populated on primitive hit
        float                   m_fPositionOnRay = -1;             // t0

        // fields required to complete intercept/hit
        CORE::Vec               m_position;             // hit position on surface of shape
        CORE::Vec               m_normal;               // normal on surface of shape
        CORE::Uv                m_uv;                   // texture coordinate on surface of shape
        uint32_t                m_uTriangleIndex = 0;   // specific triangle hit
        uint16_t                m_uTraceDepth = 0;      // number of different hits (reflections, etc.)
        uint16_t                m_uMarchDepth = 0;      // number of ray marching steps on last hit
        uint16_t                m_uIterations = 0;      // number of iterations required by last hit/step (fractal loop index, etc.)
        bool                    m_bInside = false;      // true if ray is inside shape
    };


};  // namespace SYSTEMS

