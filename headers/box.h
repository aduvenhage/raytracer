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
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial;
        }
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit, const Ray &_ray) const override {
            float t[10];
            aaboxIntersect(t, m_bounds.m_min, m_bounds.m_max, _ray.m_origin, _ray.m_direction);
            if (t[9] > 0) {
                _hit.m_pNode = this;
                _hit.m_fPositionOnRay = t[9];
                _hit.m_ray = _ray;

                return true;
            }
            
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            static const Vec _a[] = {{0, 1, 0}, {-1, 0, 0}, {0, 1, 0}};
            static const Vec _b[] = {{0, 0, 1}, {0, 0, 1}, {-1, 0, 0}};

            _hit.m_position = _hit.m_ray.position(_hit.m_fPositionOnRay);
            
            _hit.m_normal = Vec((int)(_hit.m_position.m_fX / m_vecDiv.m_fX),
                                (int)(_hit.m_position.m_fY / m_vecDiv.m_fY),
                                (int)(_hit.m_position.m_fZ / m_vecDiv.m_fZ));
            
            const int i = int(fabs(_hit.m_normal.m_fY + _hit.m_normal.m_fZ * 2) + 0.5f);
            const auto &a = _a[i];
            const auto &b = _b[i];

            const auto p2 = _hit.m_position - m_bounds.m_min;
            _hit.m_uv = Uv(a * p2 * m_fUvScale, b * p2 * m_fUvScale).wrap();
            
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

