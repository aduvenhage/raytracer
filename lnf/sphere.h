#ifndef LIBS_HEADER_SPHERE_H
#define LIBS_HEADER_SPHERE_H

#include "constants.h"
#include "node.h"
#include "vec3.h"
#include "uv.h"


namespace LNF
{
    /* Sphere shape class -- fixed at origin [0, 0, 0] */
    class Sphere        : public Node
    {
     public:
        Sphere()
            :m_fRadius(0),
             m_fRadiusSqr(0)
        {}
        
        Sphere(float _fRadius, const Material *_pMaterial)
            :m_pMaterial(_pMaterial),
             m_bounds(Vec(_fRadius, _fRadius, _fRadius), Vec(-_fRadius, -_fRadius, -_fRadius)),
             m_fRadius(_fRadius),
             m_fRadiusSqr(_fRadius * _fRadius)
        {}
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial;
        }
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit, const Ray &_ray, RandomGen &) const override {
            float dRayLength = -_ray.m_origin * _ray.m_direction;
            const float dIntersectRadiusSqr = _ray.m_origin.sizeSqr() - dRayLength*dRayLength;
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
                if ( (dRayLength >= _ray.m_fMinDist) && (dRayLength <= _ray.m_fMaxDist) ) {
                    _hit.m_pNode = this;
                    _hit.m_fPositionOnRay = dRayLength;
                    _hit.m_ray = _ray;

                    return true;
                }
            }
                    
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            _hit.m_position = _hit.m_ray.position(_hit.m_fPositionOnRay);
            _hit.m_normal = _hit.m_position / m_fRadius;

            const float phi = atan2(_hit.m_position.z(), _hit.m_position.x());
            const float theta = acos(_hit.m_position.y() / m_fRadius);
            _hit.m_uv = Uv(phi / M_PI / 2 + 0.5, theta / M_PI + 0.5);
                
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

