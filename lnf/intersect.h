#ifndef LIBS_HEADER_INTERSECT_H
#define LIBS_HEADER_INTERSECT_H

#include "constants.h"
#include "ray.h"
#include "vec3.h"
#include "uv.h"

#include <algorithm>


namespace LNF
{
    class PrimitiveInstance;

    /*
     Container for intersect attributes and products.
     */
    struct Intersect
    {
        Intersect() noexcept
            :m_pPrimitive(nullptr),
             m_fPositionOnRay(-1),
             m_uTriangleIndex(0),
             m_uTraceDepth(0),
             m_uMarchDepth(0),
             m_uIterations(0),
             m_bInside(false)
        {}

        Intersect(const Ray &_viewRay) noexcept
            :m_viewRay(_viewRay),
             m_pPrimitive(nullptr),
             m_fPositionOnRay(-1),
             m_uTriangleIndex(0),
             m_uTraceDepth(0),
             m_uMarchDepth(0),
             m_uIterations(0),
             m_bInside(false)
        {}

        Intersect(const Intersect &) noexcept = default;
        Intersect(Intersect &&) noexcept = default;
        //Intersect(Intersect &) noexcept = default;
        
        Intersect &operator=(const Intersect &) noexcept = default;
        Intersect &operator=(Intersect &&) noexcept = default;

        operator bool () const {
            return m_fPositionOnRay >= 0;
        }

        // fields populated by tracer/caller
        Ray                     m_viewRay;              // view ray
        Ray                     m_priRay;               // ray transformed for intersection with specific primitive
        const PrimitiveInstance *m_pPrimitive;          // primitive we intersected with
        
        // key fields that should be populated on primitive hit
        float                   m_fPositionOnRay;       // t0

        // fields required to complete intercept/hit
        Vec                     m_position;             // hit position on surface of shape
        Vec                     m_normal;               // normal on surface of shape
        Uv                      m_uv;                   // texture coordinate on surface of shape
        uint32_t                m_uTriangleIndex;       // specific triangle hit
        uint16_t                m_uTraceDepth;          // number of different hits (reflections, etc.)
        uint16_t                m_uMarchDepth;          // number of ray marching steps on last hit
        uint16_t                m_uIterations;          // number of iterations required by last hit/step (fractal loop index, etc.)        
        bool                    m_bInside;              // true if ray is inside shape
    };


};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_INTERSECT_H

