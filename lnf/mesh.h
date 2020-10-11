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
            Uv          m_uv[3];
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
            // find best hit
            const Triangle *pHitTriangle = nullptr;
            Intersect bestHit;
            
            // TODO: avoid copying Intersect on each new triangle hit
            for (const auto &t : m_triangles) {
                const auto &v0 = m_vertices[t.m_v[0]].m_v;
                const auto &v1 = m_vertices[t.m_v[1]].m_v;
                const auto &v2 = m_vertices[t.m_v[2]].m_v;
                
                if (triangleIntersect(_hit, v0, v1, v2, _ray) == true) {
                    if ( (pHitTriangle == nullptr) ||
                         (_hit.m_fPositionOnRay < bestHit.m_fPositionOnRay) ) {
                        pHitTriangle = &t;
                        bestHit = _hit;
                    }
                }
                
                if (pHitTriangle != nullptr)
                {
                    _hit = bestHit;
                    _hit.m_pNode = this;
                    _hit.m_normal = pHitTriangle->m_normal;
                    _hit.m_ray = _ray;
                    
                    return true;
                }
            }
            
            return false;
        }

        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            _hit.m_position = _hit.m_ray.position(_hit.m_fPositionOnRay);
            
            //calc UV
            // calc inside/outside
            
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

