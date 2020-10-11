#ifndef LIBS_HEADER_TRIANGLE_H
#define LIBS_HEADER_TRIANGLE_H

#include "constants.h"
#include "node.h"
#include "material.h"
#include "vec3.h"
#include "uv.h"


namespace LNF
{
    /*
     Triangle intersect check.
     Populates position on ray (t) and UV (barycentric) hit properties.
     
     Returns true on intersect.
     */
    bool triangleIntersect(Intersect &_hit,
                           const Vec &_v0, const Vec &_v1, const Vec &_v2,
                           const Ray &_ray) {
        const float EPSILON = 0.0000001;
        auto edge1 = _v1 - _v0;
        auto edge2 = _v2 - _v0;
        auto h = crossProduct(_ray.m_direction, edge2);
        float a = edge1 * h;
        
        if (a > -EPSILON && a < EPSILON)
            return false;    // ray is parallel to triangle.
            
        float f = 1.0/a;
        auto s = _ray.m_origin - _v0;
        float u = f * (s * h);
        if ((u < 0.0) || (u > 1.0))
            return false;
            
        auto q = crossProduct(s, edge1);
        float v = f * _ray.m_direction * q;
        if ((v < 0.0) || (u + v > 1.0))
            return false;
            
        float t = f * (edge2 * q);
        if ( (t > _ray.m_fMinDist) && (t < _ray.m_fMaxDist) ) {
            _hit.m_fPositionOnRay = t;
            _hit.m_uv = Uv(u, v);   // NOTE: Barycentric UV (u + v + w = 1)
            return true;
        }
        
        return false;
    }
    

    /* Traingle defined by 3 points  */
    class Triangle        : public Node
    {
     public:
        Triangle()
        {}

        Triangle(const Vec &_v0, const Vec &_v1, const Vec &_v2, const Material *_pMaterial)
            :m_v0(_v0),
             m_v1(_v1),
             m_v2(_v2),
             m_pMaterial(_pMaterial)
        {
            m_normal = crossProduct(m_v1 - m_v0, m_v2 - m_v0).normalized();
            m_bounds.m_min = perElementMin(perElementMin(m_v0, m_v1), m_v2) - Vec(1, 1, 1);
            m_bounds.m_max = perElementMax(perElementMax(m_v0, m_v1), m_v2) + Vec(1, 1, 1);
        }

        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial;
        }

        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit, const Ray &_ray) const override {
            if (triangleIntersect(_hit, m_v0, m_v1, m_v2, _ray) == true) {
                _hit.m_pNode = this;
                _hit.m_ray = _ray;
                return true;
            }
            
            return false;
        }

        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            _hit.m_position = _hit.m_ray.position(_hit.m_fPositionOnRay);
            _hit.m_normal = m_normal;
            return _hit;
        }

        /* returns bounds for shape */
        virtual const Bounds &bounds() const override {
            return m_bounds;
        }

     private:
        Bounds              m_bounds;
        Vec                 m_v0;
        Vec                 m_v1;
        Vec                 m_v2;
        Vec                 m_normal;
        const Material      *m_pMaterial;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_TRIANGLE_H

