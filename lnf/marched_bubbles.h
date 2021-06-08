#ifndef LIBS_HEADER_MARCHED_BUBBLES_H
#define LIBS_HEADER_MARCHED_BUBBLES_H

#include "constants.h"
#include "primitive.h"
#include "signed_distance_functions.h"
#include "trace.h"
#include "uv.h"
#include "vec3.h"


namespace LNF
{
    /* Axis aligned box shape class -- fixed at origin [0, 0, 0]; base class for raymarched nodes */
    class MarchedBubbles        : public Primitive
    {
     public:
        MarchedBubbles(const Vec &_size, const Material *_pMaterial)
            :m_bounds(-_size * 0.5f, _size * 0.5f),
             m_pMaterial(_pMaterial),
             m_fSize(_size.size() * 0.5f)
        {}

        MarchedBubbles(float _fSize, const Material *_pMaterial)
            :m_bounds(boxVec(-_fSize*0.5f), boxVec(_fSize*0.5f)),
             m_pMaterial(_pMaterial),
             m_fSize(_fSize * 0.5f)
        {}
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial;
        }

        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit, RandomGen &) const override {
            auto bi = aaboxIntersect(m_bounds, _hit.m_priRay);
            if (bi.m_intersect == true) {
                // try to hit surface inside (using raymarching)
                bool is_hit = check_marched_hit(_hit,
                                                bi.m_tmax,
                                                [this](const Vec &_p){
                                                    return sdfBubbles(_p, 0, m_fSize*2.0f);
                                                });

                if ( (is_hit == true) &&
                     (_hit.m_priRay.inside(_hit.m_fPositionOnRay) == true) )
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
            return LNF::surfaceNormal(_p, [this](const Vec &_x){
                return sdfBubbles(_x, 0, m_fSize * 2.0f);
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
        float                  m_fSize;
    };

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_MARCHED_BUBBLES_H

