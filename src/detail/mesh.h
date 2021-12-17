#ifndef DETAIL_MESH_H
#define DETAIL_MESH_H

#include "core/bvh.h"
#include "core/constants.h"
#include "core/vec3.h"
#include "core/uv.h"
#include "base/primitive.h"
#include "base/material.h"


namespace DETAIL
{
    /*
     Triangle intersect check.
     Populates position on ray (t) and UV (barycentric) hit properties.
     Returns true on intersect.
     */
    bool triangleIntersect(float &_fPositionOnRay, CORE::Uv &_uv,
                           const CORE::Ray &_ray, const CORE::Vec &_v0, const CORE::Vec &_v1, const CORE::Vec &_v2) {
        const float EPSILON = 0.000001f;
        auto edge1 = _v1 - _v0;
        auto edge2 = _v2 - _v0;
        auto h = crossProduct(_ray.m_direction, edge2);
        float a = edge1 * h;
        
        if (fabs(a) < EPSILON)
            return false;   // ray parallel to plane
            
        float f = 1.0f/a;
        auto s = _ray.m_origin - _v0;
        float u = f * (s * h);
        if ((u < 0.0) || (u > 1.0))
            return false;
            
        auto q = crossProduct(s, edge1);
        float v = f * (_ray.m_direction * q);
        if ((v < 0.0) || (u + v > 1.0))
            return false;
            
        float t = f * (edge2 * q);
        if (_ray.inside(t) == true) {
            _fPositionOnRay = t;
            _uv = CORE::Uv(u, v);   // NOTE: Barycentric UV (u + v + w = 1)
            return true;
        }
        
        return false;
    }


    /* Mesh defined by vertices, triangle indices and a material */
    class Mesh        : public BASE::Primitive
    {
     public:
        struct Triangle {
            const CORE::Bounds &bounds() const {return m_bounds;}
            
            CORE::Vec         m_normal;
            CORE::Bounds      m_bounds;
            uint32_t          m_v[3] = {};
        };
        
        struct Vertex {
            CORE::Vec         m_v;
            CORE::Uv          m_uv;
            CORE::Vec         m_normal;
        };
        
     public:
        Mesh(const BASE::Material *_pMaterial)
            :m_pMaterial(_pMaterial),
             m_bBoundsInit(false),
             m_bUseVertexNormals(false)
        {}

        /* Returns the material used for rendering, etc. */
        const BASE::Material *material() const override {
            return m_pMaterial;
        }
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(BASE::Intersect &_hit) const override {
            float fPositionOnRay = -1;
            int hitIndex = 0;
            CORE::Uv hitUv;

            if (checkBvhHit(fPositionOnRay, hitIndex, hitUv, m_bvhRoot, _hit.m_priRay) == true) {
                _hit.m_fPositionOnRay = fPositionOnRay;
                _hit.m_uTriangleIndex = hitIndex;
                _hit.m_uv = hitUv;
                return true;
            }

            return false;
        }

        /* Triangle intersect check */
        bool checkTriangleHit(float &_fPositionOnRay, int &_hitIndex, CORE::Uv &_hitUv, const Triangle *_pTriangle, const CORE::Ray &_ray) const {
            const auto &v0 = m_vertices[_pTriangle->m_v[0]];
            const auto &v1 = m_vertices[_pTriangle->m_v[1]];
            const auto &v2 = m_vertices[_pTriangle->m_v[2]];
            
            float p = 0;
            CORE::Uv uv;
            
            if (triangleIntersect(p, uv, _ray, v0.m_v, v1.m_v, v2.m_v) == true) {
                if ( (_fPositionOnRay < 0) ||
                     (p < _fPositionOnRay) )
                {
                    _fPositionOnRay = p;
                    _hitUv = uv;
                    _hitIndex = getIndex(_pTriangle);
                    return true;
                }
            }
            
            return false;
        }
        
        /* Search for best hit through BVH */
        bool checkBvhHit(float &_fPositionOnRay, int &_hitIndex, CORE::Uv &_hitUv,
                         const std::unique_ptr<CORE::BvhNode<Triangle>> &_pNode,
                         const CORE::Ray &_ray) const {
            bool bHit = false;
            
            if (_pNode->empty() == false) {
                for (const auto &pTriangle : _pNode->m_primitives) {
                    bHit |= checkTriangleHit(_fPositionOnRay, _hitIndex, _hitUv, pTriangle, _ray);
                }
            }

            if ( (_pNode->m_left != nullptr) &&
                 (_pNode->m_left->intersect(_ray) == true) ) {
                bHit |= checkBvhHit(_fPositionOnRay, _hitIndex, _hitUv, _pNode->m_left, _ray);
            }
            
            if ( (_pNode->m_right != nullptr) &&
                 (_pNode->m_right->intersect(_ray) == true) ) {
                bHit |= checkBvhHit(_fPositionOnRay, _hitIndex, _hitUv, _pNode->m_right, _ray);
            }
            
            return bHit;
        }

        /* Completes the node intersect properties. */
        virtual BASE::Intersect &intersect(BASE::Intersect &_hit) const override {
            const auto &t = m_triangles[_hit.m_uTriangleIndex];
            const auto &v0 = m_vertices[t.m_v[0]];
            const auto &v1 = m_vertices[t.m_v[1]];
            const auto &v2 = m_vertices[t.m_v[2]];
            
            _hit.m_position = _hit.m_priRay.position(_hit.m_fPositionOnRay);
            _hit.m_bInside = (_hit.m_normal * _hit.m_priRay.m_direction) >= 0;
            
            // interpolate vertex normals (from hit barycentric uv)
            if (m_bUseVertexNormals == true) {
                _hit.m_normal = _hit.m_uv.u() * v1.m_normal +
                                _hit.m_uv.v() * v2.m_normal +
                                (1 - _hit.m_uv.u() - _hit.m_uv.v()) * v0.m_normal;
            }
            else {
                _hit.m_normal = t.m_normal;
            }

            // calc texture coords (from hit barycentric uv)
            _hit.m_uv = _hit.m_uv.u() * v1.m_uv +
                        _hit.m_uv.v() * v2.m_uv +
                        (1 - _hit.m_uv.u() - _hit.m_uv.v()) * v0.m_uv;

            return _hit;
        }

        /* returns bounds for shape */
        virtual const CORE::Bounds &bounds() const override {
            return m_bounds;
        }

        /* set vertices */
        template <typename vertices_type>
        void setVertices(vertices_type &&_vertices) {
            m_vertices = std::forward<vertices_type>(_vertices);
        }

        /* set triangles */
        template <typename triangles_type>
        void setTriangles(triangles_type &&_triangles) {
            m_triangles = std::forward<triangles_type>(_triangles);
        }

        /* calc triangle normals */
        void buildTriangleNormals() {
            for (size_t i = 0; i < m_triangles.size(); i++) {
                auto &t = m_triangles[i];
                const auto &v0 = m_vertices[t.m_v[0]].m_v;
                const auto &v1 = m_vertices[t.m_v[1]].m_v;
                const auto &v2 = m_vertices[t.m_v[2]].m_v;
                
                auto e1 = v1 - v0;
                auto e2 = v2 - v0;
                t.m_normal = crossProduct(e1, e2).normalized();
            }
        }
        
        /* Calc vertex normals.
           Should be called after triangle normals have been calculated.
           Will enable the use of vertex normals (with interpolation).
         */
        void buildVertexNormals() {
            m_bUseVertexNormals = true;
            
            for (size_t i = 0; i < m_vertices.size(); i++) {
                auto &v = m_vertices[i];
                v.m_normal = CORE::Vec();
                int count = 0;
                
                for (size_t j = 0; j < m_triangles.size(); j++) {
                    auto &t = m_triangles[j];
                    
                    for (size_t k = 0; k < 3; k++) {
                        if (i == t.m_v[k]) {
                            v.m_normal += t.m_normal;
                            count++;
                        }
                    }
                }
                
                if (count > 0) {
                    v.m_normal /= (float)count;
                }
            }
        }

        /* calc mesh bounds */
        void buildBounds() {
            if (m_bBoundsInit == false) {
                // calc mesh bounds
                for (const auto &v : m_vertices) {
                    m_bounds.m_min = perElementMin(m_bounds.m_min, v.m_v);
                    m_bounds.m_max = perElementMax(m_bounds.m_max, v.m_v);
                }

                // calc triangle bounds
                for (size_t i = 0; i < m_triangles.size(); i++) {
                    auto &t = m_triangles[i];
                    const auto &v0 = m_vertices[t.m_v[0]].m_v;
                    const auto &v1 = m_vertices[t.m_v[1]].m_v;
                    const auto &v2 = m_vertices[t.m_v[2]].m_v;
                    
                    t.m_bounds.m_min = perElementMin(v0, perElementMin(v1, v2));
                    t.m_bounds.m_max = perElementMax(v0, perElementMax(v1, v2));
                }
                
                m_bBoundsInit = true;
            }
        }

        /* build acceleration structures etc. */
        void buildBvh() {
            std::vector<const Triangle*> trianglePtrs = getTrianglePtrs();
            m_bvhRoot = CORE::buildBvhRoot<2>(trianglePtrs, 32);
        }

     protected:
        // returns index of triangle in mesh
        uint32_t getIndex(const Triangle *_pTriangle) const {
            return (uint32_t)(_pTriangle - m_triangles.data());
        }
        
        // get list of triangles (raw pointers)
        std::vector<const Triangle*> getTrianglePtrs() const {
            std::vector<const Triangle*> trianglePtrs;
            trianglePtrs.reserve(m_triangles.size());
            
            for (auto &t : m_triangles) {
                trianglePtrs.push_back(&t);
            }
            
            return trianglePtrs;
        }

     private:
        std::vector<Vertex>                       m_vertices;
        std::vector<Triangle>                     m_triangles;
        CORE::Bounds                              m_bounds;
        const BASE::Material                      *m_pMaterial;
        bool                                      m_bBoundsInit;
        bool                                      m_bUseVertexNormals;
        std::unique_ptr<CORE::BvhNode<Triangle>>  m_bvhRoot;
    };


    /* Sphere built from triangles */
    class SphereMesh    : public Mesh
    {
     public:
        SphereMesh(int _iSlices, int _iDivs, float _fRadius, const BASE::Material *_pMaterial)
            :Mesh(_pMaterial)
        {
            // create vertices
            std::vector<Mesh::Vertex> vertices;
            vertices.reserve(((size_t)_iSlices+1) * ((size_t)_iDivs+1));
            
            for (int d = 0; d <= _iDivs; d++) {
                float angle = pif / _iDivs * d;
                float r = _fRadius * sin(angle);
                float y = _fRadius * cos(angle);
                
                for (int s = 0; s <= _iSlices; s++) {
                    float x = r * cos(pif*2 / _iSlices * s);
                    float z = r * sin(pif*2 / _iSlices * s);
                    
                    auto v = Mesh::Vertex();
                    v.m_v = CORE::Vec(x, y, z);
                    v.m_uv = getSphericalUv(v.m_v.normalized());
                    vertices.push_back(v);
                }
            }

            // create triangles
            std::vector<Mesh::Triangle> triangles;
            triangles.reserve((size_t)_iSlices * (size_t)_iDivs * 2);

            for (int d = 0; d < _iDivs; d++) {
                for (int s = 0; s < _iSlices; s++) {
                    int i = d * (_iSlices+1) + s;
                    
                    if (d > 0) {
                        auto t = Mesh::Triangle();
                        t.m_v[0] = (uint32_t)i;
                        t.m_v[1] = (uint32_t)i+1;
                        t.m_v[2] = (uint32_t)i+_iSlices+2;
                        triangles.push_back(t);
                    }
                    
                    if (d < _iDivs-1) {
                        auto t = Mesh::Triangle();
                        t.m_v[0] = (uint32_t)i+_iSlices+1;
                        t.m_v[1] = (uint32_t)i;
                        t.m_v[2] = (uint32_t)i+_iSlices+2;
                        triangles.push_back(t);
                    }
                }
            }
            
            setTriangles(triangles);
            setVertices(vertices);
            
            buildTriangleNormals();
            buildVertexNormals();
            buildBounds();
            buildBvh();
        }
    };
    
};  // namespace DETAIL

#endif  // #ifndef DETAIL_MESH_H

