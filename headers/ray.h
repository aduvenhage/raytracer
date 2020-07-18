#ifndef LIBS_HEADER_RAY_H
#define LIBS_HEADER_RAY_H

#include "constants.h"
#include "color.h"
#include "vec3.h"
#include "uv.h"

#include <algorithm>


namespace LNF
{
    /* ray with origin and direction */
    struct Ray
    {
        Ray() = default;
        
        Ray(const Vec &_origin, const Vec &_direction)
            :m_origin(_origin),
             m_direction(_direction)
        {}
        
        Vec position(double _dt) const {
            return m_origin + m_direction * _dt;
        }

        Vec     m_origin;
        Vec     m_direction;
    };


    /* ray with origin, direction and color */
    struct ScatteredRay
    {
        ScatteredRay() = default;
        
        ScatteredRay(const Ray &_ray, const Color &_color)
            :m_ray(_ray),
             m_color(_color)
        {}
            
        Ray     m_ray;
        Color   m_color;
    };


    /* reflect vector around normal */
    inline Vec reflect(const Vec &_vec, const Vec _normal) {
        return _vec - _normal * 2 * (_vec * _normal);
    }

    
    /* bend vector around normal with specific refraction index */
    inline Vec refract(const Vec &_vec, const Vec &_normal, double _dIndexOfRefraction)
    {
        double cosi = _vec * _normal;

        // ray entering object
        if (cosi < 0) {
            double eta = 1.0 / _dIndexOfRefraction;
            double k = 1.0 - eta * eta * (1 - cosi * cosi);
            if (k > 0.0) {
                return eta * _vec - (eta * cosi + sqrt(k)) * _normal;
            }
        }
        
        // ray exiting object
        else {
            double eta = _dIndexOfRefraction;
            double k = 1.0 - eta * eta * (1 - cosi * cosi);
            if (k > 0.0) {
                return eta * _vec - (eta * cosi - sqrt(k)) * _normal;
            }
        }
        
        return _vec;
    }


    /* calculate color mixing ration for glass */
    inline double fresnel(const Vec &_vec, const Vec &_normal, double _dIndexOfRefraction)
    {
        double cosi = _vec * _normal;
        double etai = 1.0;
        double etat = _dIndexOfRefraction;

        // check for ray exiting object
        if (cosi > 0.0) {
            std::swap(etai, etat);
        }
        else {
            cosi *= -1.0;
        }

        double sint = etai / etat * sqrt(std::max(0.0, 1.0 - cosi * cosi));
        
        // check for total internal reflection
        if (sint >= 1.0) {
            return 1.0;
        }
        else {
            double cost = sqrt(std::max(0.0, 1.0 - sint * sint));
            
            float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
            float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
            
            return (Rs * Rs + Rp * Rp) / 2;
        }
    }

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_RAY_H

