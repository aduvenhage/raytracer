#ifndef LIBS_HEADER_SPHERE_H
#define LIBS_HEADER_SPHERE_H

#include "constants.h"
#include "shape.h"
#include "vec3.h"
#include "uv.h"


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
        
        Sphere(float _dRadius, const Material *_pMaterial)
            :m_pMaterial(_pMaterial),
             m_bounds(Vec(_dRadius, _dRadius, _dRadius), Vec(-_dRadius, -_dRadius, -_dRadius)),
             m_dRadius(_dRadius),
             m_dRadiusSqr(_dRadius * _dRadius)
        {}
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial;
        }
        
        /* Returns the shape / ray intersect (calculates all hit properties). */
        virtual Intersect intersect(const Ray &_ray) const override {
            Intersect ret;
            
            float dRayLength = -_ray.m_origin * _ray.m_direction;
            const float dIntersectRadiusSqr = _ray.m_origin.sizeSqr() - dRayLength*dRayLength;
            if (dIntersectRadiusSqr <= m_dRadiusSqr) {
                const float dt = sqrt(m_dRadiusSqr - dIntersectRadiusSqr);
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

                    const float phi = atan2(ret.m_position.m_dZ, ret.m_position.m_dX);
                    const float theta = acos(ret.m_position.m_dY / m_dRadius);
                    ret.m_uv = Uv(phi / M_PI / 2 + 0.5, theta / M_PI + 0.5);
                }
            }
                
            return ret;
        }

        /* returns bounds for shape */
        virtual const Bounds &bounds() const override {
            return  m_bounds;
        }

     private:
        const Material     *m_pMaterial;
        Bounds             m_bounds;
        float              m_dRadius;
        float              m_dRadiusSqr;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_SPHERE_H

