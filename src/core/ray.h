#ifndef CORE_RAY_H
#define CORE_RAY_H

#include "constants.h"
#include "vec3.h"
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

        template <typename U, typename V>
        Ray(U &&_origin, V &&_direction, bool _bPrimary=false) noexcept
            :m_origin(std::forward<U>(_origin)),
             m_direction(std::forward<V>(_direction)),
             m_invDirection(1.0f/m_direction),
             m_fMinDist(MIN_DIST),
             m_fMaxDist(MAX_DIST),
             m_bPrimary(_bPrimary)
        {}

        Vec position(float _ft) const {
            return m_origin + m_direction * _ft;
        }
        
        bool inside(float _ft) const {
            return (_ft <= m_fMaxDist) && (_ft >= m_fMinDist);
        }

        Vec     m_origin;
        Vec     m_direction;
        Vec     m_invDirection;
        float   m_fMinDist = 0.0f;
        float   m_fMaxDist = 0.0f;
        bool    m_bPrimary = false;
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
    inline Vec refract(const Vec &_vec, const Vec &_normal, float _fIndexOfRefraction, bool _bInside) {
        float dEtaiOverEtat = _bInside ? _fIndexOfRefraction : (1.0f/_fIndexOfRefraction);
        return refract(_vec,
                       _bInside ? -_normal : _normal,
                       dEtaiOverEtat);
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
        return Ray(_axis.transformTo(_ray.m_origin), _axis.rotateTo(_ray.m_direction), _ray.m_bPrimary);
    }


    /* transform ray from space */
    inline Ray transformRayFrom(const Ray &_ray, const Axis &_axis) {
        return Ray(_axis.transformFrom(_ray.m_origin), _axis.rotateFrom(_ray.m_direction), _ray.m_bPrimary);
    }


};  // namespace CORE


#endif  // #ifndef CORE_RAY_H

