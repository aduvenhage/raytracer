#ifndef LIBS_HEADER_MARCHED_MANDLE_H
#define LIBS_HEADER_MARCHED_MANDLE_H

#include "constants.h"
#include "primitive.h"
#include "signed_distance_functions.h"
#include "trace.h"
#include "uv.h"
#include "vec3.h"


namespace LNF
{
    /* Raymarched mandlebulb -- fixed size (2.5 diameter) */
    class MarchedMandle        : public Primitive
    {
     public:
        MarchedMandle(const Material *_pMaterial)
            :m_bounds(boxVec(-1.25), boxVec(1.25)),
             m_pMaterial(_pMaterial)
        {}
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial;
        }

        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit, RandomGen &) const override {
            auto bi = aaboxIntersect(m_bounds, _hit.m_ray.m_origin, _hit.m_ray.m_invDirection);
            if (bi.m_intersect == true) {
                // try to hit surface inside (using raymarching)
                int bulbIterations = 0;
                bool is_hit = check_marched_hit(_hit,
                                                bi.m_tmax,
                                                [&](const Vec &_p){
                                                    return sdfMandle(_p, bulbIterations);
                                                });
                
                // override iteration count
                _hit.m_uIterations = (uint16_t)bulbIterations;

                if ( (is_hit == true) &&
                     ((_hit.m_fPositionOnRay >= _hit.m_ray.m_fMinDist) && (_hit.m_fPositionOnRay <= _hit.m_ray.m_fMaxDist)) )
                {
                    return true;
                }
            }
            
            return false;
        }

        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            _hit.m_normal = surfaceNormal(_hit.m_position);
            _hit.m_uv = surfaceUv(_hit.m_position);
            return _hit;
        }
 
        /* returns bounds for shape */
        virtual const Bounds &bounds() const override {
            return  m_bounds;
        }

     protected:
        // get normal from surface function
        Vec surfaceNormal(const Vec &_p) const {
            return LNF::surfaceNormal(_p, [](const Vec &_x){
                int bulbIterations = 0;
                return sdfMandle(_x, bulbIterations);
            });
        }

        // calc surface UV
        Uv surfaceUv(const Vec &_p) const {
            return getSphericalUv(_p, _p.size());
        }

     private:
        Axis                   m_axis;
        Bounds                 m_bounds;
        const Material         *m_pMaterial;
    };

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_MARCHED_MANDLE_H

