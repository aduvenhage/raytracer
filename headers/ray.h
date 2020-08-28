#ifndef LIBS_HEADER_RAY_H
#define LIBS_HEADER_RAY_H

#include "constants.h"
#include "color.h"
#include "vec3.h"
#include "uv.h"

#include <algorithm>
#include <limits>
#include <random>


namespace LNF
{
    /* ray with origin and direction */
    struct Ray
    {
        Ray() = default;
        Ray(const Ray &) = default;
        Ray(Ray &&) = default;
        Ray(Ray &) = default;

        template <typename U>
        Ray(U &&_direction)
            :m_direction(std::forward<U>(_direction)),
             m_dMinDist(0.00001),
             m_dMaxDist(std::numeric_limits<double>::max())
        {}
        
        template <typename U, typename V>
        Ray(U &&_origin, V &&_direction)
            :m_origin(std::forward<U>(_origin)),
             m_direction(std::forward<V>(_direction)),
             m_dMinDist(0.00001),
             m_dMaxDist(std::numeric_limits<double>::max())
        {}

        Ray &operator=(const Ray &) = default;
        Ray &operator=(Ray &&) = default;
        
        Vec position(double _dt) const {
            return m_origin + m_direction * _dt;
        }

        Vec     m_origin;
        Vec     m_direction;
        double  m_dMinDist;
        double  m_dMaxDist;
    };


    /* ray with origin, direction and color */
    struct ScatteredRay
    {
        ScatteredRay() = default;
        
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
    double schlick(double cosi, double _dEtaiOverEtat) {
        // https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
        auto r0 = sqr((1 - _dEtaiOverEtat) / (1 + _dEtaiOverEtat));
        return r0 + (1 - r0) * pow((1 - cosi), 5);
    }


    /* bend vector around normal with specific refraction index */
    inline Vec refract(const Vec &_vec, const Vec &_normal, double _dEtaiOverEtat, RandomGen &_randomGen)
    {
        // https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
        double cosi = -_vec * _normal;
        double k = sqr(_dEtaiOverEtat) * (1 - sqr(cosi));
        std::uniform_real_distribution<double> uniform01(0, 1);
        
        // k > 1 ==> total internal reflection
        if ( (k > 1) ||
            (uniform01(_randomGen) < schlick(cosi, _dEtaiOverEtat)) )
        {
            // total internal reflection
            return _vec + _normal * 2 * cosi;
        }
        else {
            // refraction
            return _dEtaiOverEtat * _vec + (_dEtaiOverEtat * cosi - sqrt(1 - k)) * _normal;
        }
    }

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_RAY_H

