#ifndef LIBS_HEADER_RAY_H
#define LIBS_HEADER_RAY_H

#include "constants.h"
#include "vec3.h"


namespace LNF
{
    struct Ray
    {
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


    struct Intersect
    {
        Intersect()
            :m_dPositionOnRay(0)
        {}

        Intersect(const Ray &_ray, double _dPositionOnRay)
            :m_dPositionOnRay(_dPositionOnRay),
             m_position(_ray.position(_dPositionOnRay))
        {}
        
        operator bool () const {
            return m_dPositionOnRay > 0;
        }

        double  m_dPositionOnRay;
        Vec     m_position;
    };


    inline Vec reflect(const Vec &_vec, const Vec _normal) {
        // refl = ray - nhit * 2 * (ray . nhit)
        return _vec - _normal * 2 * (_vec * _normal);
    }

    
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

