#ifndef DETAIL_BOX_H
#define DETAIL_BOX_H

#include "core/constants.h"
#include "core/ray.h"
#include "core/vec3.h"
#include "core/uv.h"
#include "base/primitive.h"


namespace DETAIL
{
    /* Axis aligned box shape class -- fixed at origin [0, 0, 0] */
    class Box        : public BASE::Primitive
    {
     public:
        Box()
        {}
        
        Box(const CORE::Vec &_size, const BASE::Material *_pMaterial, float _fUvScale = 0.2f)
            :m_bounds(-_size * 0.5f, _size * 0.5f),
             m_vecDiv(_size * 0.49999f),
             m_pMaterial(_pMaterial),
             m_fUvScale(_fUvScale)
        {}
        
        Box(float _fSize, const BASE::Material *_pMaterial, float _fUvScale = 0.2f)
            :m_bounds(CORE::boxVec(-_fSize*0.5f), CORE::boxVec(_fSize*0.5f)),
             m_vecDiv(CORE::boxVec(_fSize*0.49999f)),
             m_pMaterial(_pMaterial),
             m_fUvScale(_fUvScale)
        {}
        
        /* Returns the material used for rendering, etc. */
        const BASE::Material *material() const override {
            return m_pMaterial;
        }
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(BASE::Intersect &_hit) const override {
            auto bi = aaboxIntersect(m_bounds, _hit.m_priRay);
            if (bi.m_intersect == true) {
                auto t = bi.m_inside ? bi.m_tmax : bi.m_tmin;
                if (_hit.m_priRay.inside(t) == true) {
                    _hit.m_fPositionOnRay = t;
                    _hit.m_bInside = bi.m_inside;

                    return true;
                }
            }
            
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual BASE::Intersect &intersect(BASE::Intersect &_hit) const override {
            _hit.m_position = _hit.m_priRay.position(_hit.m_fPositionOnRay);
            
            _hit.m_normal = CORE::Vec(trunc(_hit.m_position.x() / m_vecDiv.x()),
                                trunc(_hit.m_position.y() / m_vecDiv.y()),
                                trunc(_hit.m_position.z() / m_vecDiv.z()));
            
            auto e1 = crossProduct(_hit.m_normal, CORE::Vec(0.0f, 0.0f, 1.0f));
            if (e1.sizeSqr() < 0.0001f) {
                e1 = crossProduct(_hit.m_normal, CORE::Vec(0.0f, 1.0f, 0.0f));
            }
            
            e1 = e1.normalized();
            auto e2 = crossProduct(_hit.m_normal, e1);

            const auto p2 = _hit.m_position - m_bounds.m_min;
            _hit.m_uv = CORE::Uv(e1 * p2 * m_fUvScale, e2 * p2 * m_fUvScale).wrap();
            
            return _hit;
        }
                
        /* returns bounds for shape */
        virtual const CORE::Bounds &bounds() const override {
            return  m_bounds;
        }
        
     private:
        CORE::Axis             m_axis;
        CORE::Bounds           m_bounds;
        CORE::Vec              m_vecDiv;
        const BASE::Material   *m_pMaterial;
        float                  m_fUvScale;
    };


};  // namespace DETAIL

#endif  // #ifndef DETAIL_BOX_H

