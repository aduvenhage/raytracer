#ifndef LIBS_HEADER_BOX_H
#define LIBS_HEADER_BOX_H

#include "constants.h"
#include "node.h"
#include "vec3.h"
#include "uv.h"


namespace LNF
{
    /* Axis aligned box shape class -- fixed at origin [0, 0, 0] */
    class Box        : public Node
    {
     public:
        Box()
        {}
        
        Box(const Vec &_size, const Material *_pMaterial, float _fUvScale = 0.2f)
            :m_bounds(-_size * 0.5f, _size * 0.5f),
             m_vecDiv(_size * 0.49999f),
             m_pMaterial(_pMaterial),
             m_fUvScale(_fUvScale)
        {}
        
        Box(float _fSize, const Material *_pMaterial, float _fUvScale = 0.2f)
            :m_bounds(boxVec(-_fSize*0.5), boxVec(_fSize*0.5)),
             m_vecDiv(boxVec(_fSize*0.49999f)),
             m_pMaterial(_pMaterial),
             m_fUvScale(_fUvScale)
        {}
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial;
        }
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit, const Ray &_ray, RandomGen &) const override {
            auto bi = aaboxIntersect(m_bounds, _ray.m_origin, _ray.m_invDirection);
            if (bi.m_intersect == true) {
                auto t = bi.m_inside ? bi.m_tmax : bi.m_tmin;
                if ( (t >= _ray.m_fMinDist) && (t <= _ray.m_fMaxDist) ) {
                    _hit.m_pNode = this;
                    _hit.m_fPositionOnRay = t;
                    _hit.m_ray = _ray;
                    _hit.m_bInside = bi.m_inside;

                    return true;
                }
            }
            
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            _hit.m_position = _hit.m_ray.position(_hit.m_fPositionOnRay);
            
            _hit.m_normal = Vec((int)(_hit.m_position.x() / m_vecDiv.x()),
                                (int)(_hit.m_position.y() / m_vecDiv.y()),
                                (int)(_hit.m_position.z() / m_vecDiv.z()));
            
            auto e1 = crossProduct(_hit.m_normal, Vec(0.0f, 0.0f, 1.0f));
            if (e1.sizeSqr() < 0.0001f) {
                e1 = crossProduct(_hit.m_normal, Vec(0.0f, 1.0f, 0.0f));
            }
            
            e1 = e1.normalized();
            auto e2 = crossProduct(_hit.m_normal, e1);

            const auto p2 = _hit.m_position - m_bounds.m_min;
            _hit.m_uv = Uv(e1 * p2 * m_fUvScale, e2 * p2 * m_fUvScale).wrap();
            
            return _hit;
        }
                
        /* returns bounds for shape */
        virtual const Bounds &bounds() const override {
            return  m_bounds;
        }
        
     private:
        Axis                   m_axis;
        Bounds                 m_bounds;
        Vec                    m_vecDiv;
        const Material         *m_pMaterial;
        float                  m_fUvScale;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_BOX_H

