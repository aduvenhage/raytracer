#ifndef LIBS_HEADER_SPHERE_H
#define LIBS_HEADER_SPHERE_H

#include "constants.h"
#include "shape.h"

#include <memory>


namespace LNF
{
    /* Sphere shape class -- fixed at origin [0, 0, 0] */
    class Sphere        : public Shape
    {
     public:
        Sphere()
            :m_dRadius(0),
             m_dRadiusSqr(0)
        {}
        
        Sphere(double _dRadius, const std::shared_ptr<Material> &_pMaterial)
            :m_pMaterial(_pMaterial),
             m_dRadius(_dRadius),
             m_dRadiusSqr(_dRadius * _dRadius)
        {}
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial.get();
        }
        
        /* Returns the shape / ray intersect (calculates all hit properties). */
        virtual Intersect intersect(const Ray &_ray) const override {
            Intersect ret;
            
            double dRayLength = -_ray.m_origin * _ray.m_direction;
            const double dIntersectRadiusSqr = _ray.m_origin.sizeSqr() - dRayLength*dRayLength;            
            if (dIntersectRadiusSqr <= m_dRadiusSqr) {
                const double dt = sqrt(m_dRadiusSqr - dIntersectRadiusSqr);
                if (dt <= dRayLength) {
                    // we are outside of sphere
                    dRayLength -= dt;
                    ret.m_bInside = false;
                }
                else {
                    // we are inside of sphere
                    dRayLength += dt;
                    ret.m_bInside = true;
                }
                
                // check ray limits
                if ( (dRayLength >= _ray.m_dMinDist) && (dRayLength <= _ray.m_dMaxDist) ) {
                    ret.m_pShape = this;
                    ret.m_dPositionOnRay = dRayLength;
                    ret.m_position = _ray.position(ret.m_dPositionOnRay);
                    ret.m_normal = ret.m_position / m_dRadius;

                    const double phi = atan2(ret.m_position.m_dZ, ret.m_position.m_dX);
                    const double theta = acos(ret.m_position.m_dY / m_dRadius);
                    ret.m_uv = Uv(phi / M_PI / 2 + 0.5, theta / M_PI + 0.5);
                }
            }
                
            return ret;
        }
         
     private:
        std::shared_ptr<Material>   m_pMaterial;
        double                      m_dRadius;
        double                      m_dRadiusSqr;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_SPHERE_H

