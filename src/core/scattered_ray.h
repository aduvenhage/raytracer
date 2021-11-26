#ifndef CORE_SCATTERED_RAY_H
#define CORE_SCATTERED_RAY_H

#include "color.h"
#include "ray.h"


namespace CORE
{
    /* ray with origin, direction and color */
    struct ScatteredRay
    {
		ScatteredRay() noexcept = default;

        template <typename R, typename CC, typename CE>
        ScatteredRay(R &&_ray, CC &&_color, CE &&_emitted) noexcept
            :m_ray(std::forward<R>(_ray)),
             m_color(std::forward<CC>(_color)),
             m_emitted(std::forward<CE>(_emitted))
        {}
            
        Ray     m_ray;
        Color   m_color = CORE::COLOR::White;
        Color   m_emitted;
    };

};  // namespace CORE


#endif  // #ifndef CORE_SCATTERED_RAY_H

