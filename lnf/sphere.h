#ifndef LIBS_HEADER_SPHERE_H
#define LIBS_HEADER_SPHERE_H

#include "constants.h"
#include "primitive.h"
#include "vec3.h"
#include "uv.h"


namespace LNF
{
    /* Sphere shape class -- fixed at origin [0, 0, 0] */
    class Sphere        : public Primitive
    {
     public:
        Sphere()
            :m_fRadius(0),
             m_fRadiusSqr(0)
        {}
        
        Sphere(float _fRadius, const Material *_pMaterial)
            :m_pMaterial(_pMaterial),
             m_bounds(Vec(-_fRadius, -_fRadius, -_fRadius), Vec(_fRadius, _fRadius, _fRadius)),
             m_fRadius(_fRadius),
             m_fRadiusSqr(_fRadius * _fRadius)
        {}
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial;
        }
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit) const override {
            float dRayLength = -_hit.m_priRay.m_origin * _hit.m_priRay.m_direction;
            const float dIntersectRadiusSqr = _hit.m_priRay.m_origin.sizeSqr() - dRayLength*dRayLength;
            if (dIntersectRadiusSqr <= m_fRadiusSqr) {
                const float dt = sqrt(m_fRadiusSqr - dIntersectRadiusSqr);
                if (dt <= dRayLength) {
                    // we are outside of sphere
                    dRayLength -= dt;
                    _hit.m_bInside = false;
                }
                else {
                    // we are inside of sphere
                    dRayLength += dt;
                    _hit.m_bInside = true;
                }

                // check ray limits
                if (_hit.m_priRay.inside(dRayLength) == true) {
                    _hit.m_fPositionOnRay = dRayLength;
                    return true;
                }
            }
                    
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            _hit.m_position = _hit.m_priRay.position(_hit.m_fPositionOnRay);
            _hit.m_normal = _hit.m_position / m_fRadius;
            _hit.m_uv = getSphericalUv(_hit.m_position, m_fRadius);

            return _hit;
        }

        /* returns bounds for shape */
        virtual const Bounds &bounds() const override {
            return  m_bounds;
        }

     private:
        const Material     *m_pMaterial;
        Bounds             m_bounds;
        float              m_fRadius;
        float              m_fRadiusSqr;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_SPHERE_H

