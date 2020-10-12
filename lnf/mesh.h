#ifndef LIBS_HEADER_MESH_H
#define LIBS_HEADER_MESH_H

#include "constants.h"
#include "node.h"
#include "material.h"
#include "triangle.h"
#include "vec3.h"
#include "uv.h"


namespace LNF
{
    /* Mesh defined by vertices, triangle indices and a material */
    class Mesh        : public Node
    {
     public:
        struct Triangle {
            Vec         m_normal;
            uint32_t    m_v[3];
        };
        
        struct Vertex {
            Vec         m_v;
            Uv          m_uv;
        };
        
     public:
        Mesh()
        {}

        Mesh(const std::vector<Vertex> &_vertices, const std::vector<Triangle> &_triangles, const Material *_pMaterial)
            :m_vertices(_vertices),
             m_triangles(_triangles),
             m_pMaterial(_pMaterial)
        {
            // calc bounds
            for (const auto &v : m_vertices) {
                m_bounds.m_min = perElementMin(m_bounds.m_min, v.m_v);
                m_bounds.m_max = perElementMax(m_bounds.m_max, v.m_v);
            }

            // calc triangle normals
            for (auto &t : m_triangles) {
                const auto &v0 = m_vertices[t.m_v[0]].m_v;
                const auto &v1 = m_vertices[t.m_v[1]].m_v;
                const auto &v2 = m_vertices[t.m_v[2]].m_v;
                
                auto e1 = v1 - v0;
                auto e2 = v2 - v0;
                t.m_normal = crossProduct(e1, e2).normalized();
            }
        }

        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial;
        }

        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit, const Ray &_ray) const override {
            Intersect hits[2];
            Intersect *pNewHit = hits + 0;
            Intersect *pBestHit = hits + 1;
            uint32_t hitCount = 0;
            
            
            for (size_t i = 0; i < m_triangles.size(); i++) {
                const auto &t = m_triangles[i];
                const auto &v0 = m_vertices[t.m_v[0]];
                const auto &v1 = m_vertices[t.m_v[1]];
                const auto &v2 = m_vertices[t.m_v[2]];
                
                if (triangleIntersect(*pNewHit, v0.m_v, v1.m_v, v2.m_v, _ray) == true) {
                    if ( (hitCount == 0) ||
                         (pNewHit->m_fPositionOnRay < pBestHit->m_fPositionOnRay) )
                    {
                        std::swap(pNewHit, pBestHit);
                        pBestHit->m_uTriangleIndex = i;
                        hitCount++;
                    }
                }
                
                if (hitCount > 0)
                {
                    _hit = *pBestHit;
                    _hit.m_pNode = this;
                    _hit.m_normal = m_triangles[_hit.m_uTriangleIndex].m_normal;
                    _hit.m_ray = _ray;
                    
                    return true;
                }
            }
            
            return false;
        }

        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            const auto &t = m_triangles[_hit.m_uTriangleIndex];
            const auto &v0 = m_vertices[t.m_v[0]];
            const auto &v1 = m_vertices[t.m_v[1]];
            const auto &v2 = m_vertices[t.m_v[2]];
            
            _hit.m_position = _hit.m_ray.position(_hit.m_fPositionOnRay);
            _hit.m_bInside = (_hit.m_normal * _hit.m_ray.m_direction) > 0;
            _hit.m_uv = _hit.m_uv.u() * v0.m_uv + _hit.m_uv.v() * v1.m_uv + (1 - _hit.m_uv.u() - _hit.m_uv.v()) * v2.m_uv;
            
            return _hit;
        }

        /* returns bounds for shape */
        virtual const Bounds &bounds() const override {
            return m_bounds;
        }

     private:
        std::vector<Vertex>     m_vertices;
        std::vector<Triangle>   m_triangles;
        Bounds                  m_bounds;
        const Material          *m_pMaterial;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_TRIANGLE_H

