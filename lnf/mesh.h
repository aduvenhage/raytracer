#ifndef LIBS_HEADER_MESH_H
#define LIBS_HEADER_MESH_H

#include "bvh.h"
#include "constants.h"
#include "primitive.h"
#include "material.h"
#include "triangle.h"
#include "vec3.h"
#include "uv.h"


namespace LNF
{
    /* Mesh defined by vertices, triangle indices and a material */
    class Mesh        : public Primitive
    {
     public:
        struct Triangle {
            const Bounds &bounds() {return m_bounds;}
            
            Vec         m_normal;
            Bounds      m_bounds;
            uint32_t    m_v[3];
            uint32_t    m_index;
        };
        
        struct Vertex {
            Vec         m_v;
            Uv          m_uv;
        };
        
     public:
        Mesh()
        {}

        template <typename vertices_type, typename triangles_type>
        Mesh(vertices_type &&_vertices, triangles_type &_triangles, const Material *_pMaterial)
            :m_vertices(std::forward<vertices_type>(_vertices)),
             m_triangles(std::forward<triangles_type>(_triangles)),
             m_pMaterial(_pMaterial)
        {
            // calc mesh bounds
            for (const auto &v : m_vertices) {
                m_bounds.m_min = perElementMin(m_bounds.m_min, v.m_v);
                m_bounds.m_max = perElementMax(m_bounds.m_max, v.m_v);
            }

            // calc triangle normals and bounds
            for (size_t i = 0; i < m_triangles.size(); i++) {
                auto &t = m_triangles[i];
                const auto &v0 = m_vertices[t.m_v[0]].m_v;
                const auto &v1 = m_vertices[t.m_v[1]].m_v;
                const auto &v2 = m_vertices[t.m_v[2]].m_v;
                
                auto e1 = v1 - v0;
                auto e2 = v2 - v0;
                t.m_normal = crossProduct(e1, e2).normalized();
                t.m_bounds.m_min = perElementMin(v0, perElementMin(v1, v2));
                t.m_bounds.m_max = perElementMax(v0, perElementMax(v1, v2));
                t.m_index = (uint32_t)i;
            }
            
            // build BVH
            std::vector<Triangle*> trianglePtrs;
            trianglePtrs.reserve(m_triangles.size());
            
            for (auto &t : m_triangles) {
                trianglePtrs.push_back(&t);
            }
            
            
            
            
            
            
            
            
            
            
            //m_bvhRoot.build(trianglePtrs);
            
            
            
            
            
        }

        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial;
        }

        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit, RandomGen &) const override {
            static thread_local std::vector<Triangle*> nodes;
            
            nodes.clear();
            m_bvhRoot.intersect(nodes, _hit.m_ray);

            Intersect newHit;
            bool bHit = false;
            
            for (auto *pTriangle : nodes) {
                const auto &v0 = m_vertices[pTriangle->m_v[0]];
                const auto &v1 = m_vertices[pTriangle->m_v[1]];
                const auto &v2 = m_vertices[pTriangle->m_v[2]];
                
                if (triangleIntersect(newHit, v0.m_v, v1.m_v, v2.m_v) == true) {
                    if ( (bHit == false) ||
                         (newHit.m_fPositionOnRay < _hit.m_fPositionOnRay) )
                    {
                        _hit = newHit;
                        _hit.m_uTriangleIndex = pTriangle->m_index;
                        bHit = true;
                    }
                }
            }
                
            if (bHit == true) {
                return true;
            }
            else {
                return false;
            }
        }

        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            const auto &t = m_triangles[_hit.m_uTriangleIndex];
            const auto &v0 = m_vertices[t.m_v[0]];
            const auto &v1 = m_vertices[t.m_v[1]];
            const auto &v2 = m_vertices[t.m_v[2]];
            
            _hit.m_position = _hit.m_ray.position(_hit.m_fPositionOnRay);
            _hit.m_normal = m_triangles[_hit.m_uTriangleIndex].m_normal;
            _hit.m_bInside = (_hit.m_normal * _hit.m_ray.m_direction) >= 0;
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
        BvhNode<Triangle>       m_bvhRoot;
        Bounds                  m_bounds;
        const Material          *m_pMaterial;
    };


    std::unique_ptr<Mesh> buildSphereMesh(int _iSlices, int _iDivs, float _fRadius, const Material *_pMaterial) {
        std::vector<Mesh::Vertex> vertices;
        std::vector<Mesh::Triangle> triangles;
        
        vertices.reserve((_iSlices+1) * (_iDivs+1));
        triangles.reserve(_iSlices * _iDivs * 2);
        
        for (int d = 0; d <= _iDivs; d++) {
            float angle = M_PI / _iDivs * d;
            float y = _fRadius * cos(angle);
            float r = _fRadius * sin(angle);
            
            for (int s = 0; s <= _iSlices; s++) {
                float x = r * cos(M_PI / _iSlices * s * 2);
                float z = r * sin(M_PI / _iSlices * s * 2);
                
                auto v = Mesh::Vertex();
                v.m_v = Vec(x, y, z);
                
                vertices.push_back(v);
                
                if ( (d > 0) && (s > 0) ) {
                    auto i = vertices.size() - 1;
                    
                    if (d > 1) {
                        auto t1 = Mesh::Triangle();
                        t1.m_v[0] = (uint32_t)i-_iSlices-2;
                        t1.m_v[1] = (uint32_t)i-_iSlices-1;
                        t1.m_v[2] = (uint32_t)i-1;
                        triangles.push_back(t1);
                    }
                    
                    if (d < _iDivs) {
                        auto t2 = Mesh::Triangle();
                        t2.m_v[0] = (uint32_t)i-_iSlices-1;
                        t2.m_v[1] = (uint32_t)i;
                        t2.m_v[2] = (uint32_t)i-1;
                        triangles.push_back(t2);
                    }
                }
            }
        }
        
        return std::make_unique<Mesh>(vertices, triangles, _pMaterial);
    }

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_TRIANGLE_H

