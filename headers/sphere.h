#ifndef LIBS_HEADER_SPHERE_H
#define LIBS_HEADER_SPHERE_H

#include "constants.h"
#include "shape.h"

#include <memory>


namespace LNF
{
    /* Spehere class */
    class Sphere        : public Shape
    {
     public:
        Sphere()
            :m_dRadius(0),
             m_dRadiusSqr(0)
        {}
        
        Sphere(const Vec &_origin, double _dRadius, std::unique_ptr<Material> _pMaterial)
            :m_origin(_origin),
             m_pMaterial(std::move(_pMaterial)),
             m_dRadius(_dRadius),
             m_dRadiusSqr(_dRadius * _dRadius)
        {}
        
        /*
         Returns the point (t) on the ray where it intersects this shape.
         Returns 0.0 if there is no intersect possible.
         */
        virtual double intersect(const Ray &_ray) const {
            auto vecRaySphere = m_origin - _ray.m_origin;
            double dRayLength = vecRaySphere * _ray.m_direction;
            if (dRayLength <= 0){
                return 0.0;
            }
            
            double dIntersectRadiusSqr = vecRaySphere.sizeSqr() - dRayLength*dRayLength;
            if (dIntersectRadiusSqr > m_dRadiusSqr){
                return 0.0;
            }
            
            double dt = sqrt(m_dRadiusSqr - dIntersectRadiusSqr);
            if (dt <= dRayLength) {
                return dRayLength - dt;
            }
            else {
                return dRayLength + dt;
            }
        }
        
        /* Returns the shape normal vector at the given surface position. */
        virtual Vec normal(const Vec &_pos) const
        {
            return (_pos - m_origin).normalize();
        }

        /* Returns texture coordinates on given surface position. */
        virtual Uv uv(const Vec &_pos) const {
            auto vec = _pos - m_origin;
            double phi = atan2(vec.m_dZ, vec.m_dX);
            double theta = acos(vec.m_dY / m_dRadius);

            return Uv(phi / M_PI / 2 + 0.5, theta / M_PI + 0.5);
        }
        
        /* Returns the shape material */
        virtual Material *material() const {
            return m_pMaterial.get();
        }
        
        Vec                         m_origin;
        std::unique_ptr<Material>   m_pMaterial;
        double                      m_dRadius;
        double                      m_dRadiusSqr;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_SPHERE_H

