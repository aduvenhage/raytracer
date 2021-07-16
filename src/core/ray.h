#ifndef CORE_RAY_H
#define CORE_RAY_H

#include "constants.h"
#include "color.h"
#include "vec3.h"
#include "uv.h"
#include "random.h"

#include <algorithm>
#include <limits>
#include <random>


namespace CORE
{
    /* ray with origin and direction */
    struct Ray
    {
        static constexpr float MIN_DIST = 1e-4f;
        static constexpr float MAX_DIST = std::numeric_limits<float>::max();
        
        Ray() noexcept = default;
        Ray(const Ray &) noexcept = default;
        Ray(Ray &&) noexcept = default;
        //Ray(Ray &) noexcept = default;

        template <typename U, typename V>
        Ray(U &&_origin, V &&_direction) noexcept
            :m_origin(std::forward<U>(_origin)),
             m_direction(std::forward<V>(_direction)),
             m_invDirection(1/_direction),
             m_fMinDist(MIN_DIST),
             m_fMaxDist(MAX_DIST)
        {}

        Ray &operator=(const Ray &) noexcept = default;
        Ray &operator=(Ray &&) noexcept = default;
        
        Vec position(float _ft) const {
            return m_origin + m_direction * _ft;
        }
        
        bool inside(float _ft) const {
            return (_ft <= m_fMaxDist) && (_ft >= m_fMinDist);
        }

        Vec     m_origin;
        Vec     m_direction;
        Vec     m_invDirection;
        float   m_fMinDist;
        float   m_fMaxDist;
    };


    /* ray with origin, direction and color */
    struct ScatteredRay
    {
        ScatteredRay() = default;
        ScatteredRay(const ScatteredRay &) = default;
        ScatteredRay(ScatteredRay &&) = default;
        //ScatteredRay(ScatteredRay &) = default;
        
        template <typename R, typename CC, typename CE>
        ScatteredRay(R &&_ray, CC &&_color, CE &&_emitted)
            :m_ray(std::forward<R>(_ray)),
             m_color(std::forward<CC>(_color)),
             m_emitted(std::forward<CE>(_emitted))
        {}
            
        Ray     m_ray;
        Color   m_color;
        Color   m_emitted;
    };


    /* reflect vector around normal */
    inline Vec reflect(const Vec &_vec, const Vec _normal) {
        // https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
        return _vec - _normal * 2 * (_vec * _normal);
    }


    /* Schlick’s approximation to Fresnel */
    float schlick(float cosi, float _fEtaiOverEtat) {
        // https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
        auto r0 = sqr((1 - _fEtaiOverEtat) / (1 + _fEtaiOverEtat));
        return r0 + (1 - r0) * pow(1 - cosi, 5.0f);
    }


    /* bend vector around normal with specific refraction index */
    inline Vec refract(const Vec &_vec, const Vec &_normal, float _fEtaiOverEtat)
    {
        // https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
        float cosi = -_vec * _normal;
        float k = sqr(_fEtaiOverEtat) * (1 - sqr(cosi));
        std::uniform_real_distribution<float> uniform01(0, 1);
        
        // k > 1 ==> total internal reflection
        if ( (k > 1) ||
             (uniform01(generator()) < schlick(cosi, _fEtaiOverEtat)) )
        {
            // total internal reflection
            return _vec + _normal * 2 * cosi;
        }
        else {
            // refraction
            return _fEtaiOverEtat * _vec + (_fEtaiOverEtat * cosi - sqrt(1 - k)) * _normal;
        }
    }


    /* glass like reflection/refraction (includes surface scatter and inside/outside checks) */
    inline Vec refract(const Vec &_vec, const Vec &_normal, float _fIndexOfRefraction, bool _bInside, float _fScatter) {
        float dEtaiOverEtat = _bInside ? _fIndexOfRefraction : (1.0f/_fIndexOfRefraction);
        auto normal = (_normal + randomUnitSphere() * _fScatter).normalized();
        normal *= _bInside ? -1.0f : 1.0f;
        
        return refract(_vec, normal, dEtaiOverEtat);
    }


    /* ray-box intersection (_invDir = 1 / ray_direction) */
    inline bool aaboxIntersectCheck(const Bounds &_box, const Ray &_ray) {
        return aaboxIntersectCheck(_box, _ray.m_origin, _ray.m_invDirection);
    }


    /* ray-box intersection (_invDir = 1 / ray_direction) */
    inline AABoxItersect aaboxIntersect(const Bounds &_box, const Ray &_ray) {
        return aaboxIntersect(_box, _ray.m_origin, _ray.m_invDirection);
    }


    /* transform ray to space */
    inline Ray transformRayTo(const Ray &_ray, const Axis &_axis) {
        return Ray(_axis.transformTo(_ray.m_origin), _axis.rotateTo(_ray.m_direction));
    }


    /* transform ray from space */
    inline Ray transformRayFrom(const Ray &_ray, const Axis &_axis) {
        return Ray(_axis.transformFrom(_ray.m_origin), _axis.rotateFrom(_ray.m_direction));
    }


};  // namespace CORE


#endif  // #ifndef CORE_RAY_H

