#pragma once

#include "core/constants.h"
#include "core/vec3.h"
#include "core/uv.h"
#include "base/bvh.h"
#include "base/primitive.h"
#include "base/material.h"

#include <queue>


namespace DETAIL
{
    struct MeshTriangle {
        const CORE::Bounds &bounds() const {return m_bounds;}
        
        CORE::Vec         m_normal;
        CORE::Bounds      m_bounds;
        uint32_t          m_v[3] = {};
    };
    
    struct MeshVertex {
        CORE::Vec         m_v;
        CORE::Uv          m_uv;
        CORE::Vec         m_normal;
    };
    
    // output of traingle intersection test
    struct MeshIntersect
    {
        MANAGE_MEMORY('RSCN')

        MeshIntersect() noexcept = default;
        
        operator bool () const {
            return m_fPositionOnRay >= 0;
        }

        bool operator < (const MeshIntersect &_rhs) const {
            return m_fPositionOnRay < _rhs.m_fPositionOnRay;
        }

        bool operator > (const MeshIntersect &_rhs) const {
            return m_fPositionOnRay > _rhs.m_fPositionOnRay;
        }

        BASE::BvhNode<MeshTriangle> *m_pNode = nullptr;
        CORE::Uv m_uv;
        int32_t m_iTriangleIndex = -1;
        float m_fPositionOnRay = -1;
    };


    /*
     Triangle intersect check.
     Populates position on ray (t) and UV (barycentric) hit properties.
     Returns true on intersect.
     */
    MeshIntersect triangleIntersect(const CORE::Ray &_ray, const CORE::Vec &_v0, const CORE::Vec &_v1, const CORE::Vec &_v2) {
        MeshIntersect tri;
        const float EPSILON = 0.000001f;
        auto edge1 = _v1 - _v0;
        auto edge2 = _v2 - _v0;
        auto h = crossProduct(_ray.m_direction, edge2);
        float a = edge1 * h;
        
        if (fabs(a) >= EPSILON) {
            float f = 1.0f/a;
            auto s = _ray.m_origin - _v0;
            float u = f * (s * h);
            if ((u >= 0.0) && (u <= 1.0))
            {
                auto q = crossProduct(s, edge1);
                float v = f * (_ray.m_direction * q);
                if ((v >= 0.0) && (u + v <= 1.0))
                {
                    float t = f * (edge2 * q);
                    if (_ray.inside(t) == true) {
                        tri.m_fPositionOnRay = t;
                        tri.m_uv = CORE::Uv(u, v);   // NOTE: Barycentric UV (u + v + w = 1)
                    }
                }
            }
        }

        return tri;
    }
    
        
    /* Mesh defined by vertices, triangle indices and a material */
    class Mesh        : public BASE::Primitive
    {
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
            MeshIntersect triHit;
            uint32_t objectHits = 0;
            uint32_t boxHits = 0;
            
            /*
            boxHits = BASE::checkBvhHit(m_pBvhRoot, _hit.m_priRay,
                              [&](const MeshTriangle *_pTriangle, const CORE::Ray &_ray){
                                    MeshIntersect nh = getMeshIntersect(_pTriangle, _ray);
                                    if (nh == true) {
                                        objectHits++;
                                        
                                        if ( (triHit == false) ||
                                             (nh < triHit) )
                                        {
                                            triHit = nh;
                                        }
                                    }
                              });
            
            */
            
            
            std::priority_queue<MeshIntersect, std::vector<MeshIntersect>, std::greater<MeshIntersect>> queue;
            MeshIntersect root;
            root.m_pNode = m_pBvhRoot;
            queue.push(root);

            while (queue.empty() == false) {
                MeshIntersect node = queue.top();
                queue.pop();
                
                if (node.m_iTriangleIndex >= 0) {
                    // we have our hit
                    triHit = node;
                    break;
                }
                else {
                    BASE::BvhNode<MeshTriangle> *pBvhNode = node.m_pNode;
                    if (pBvhNode->m_pPrimitive != nullptr) {
                        objectHits++;
                        MeshIntersect nh = getMeshIntersect(pBvhNode->m_pPrimitive, _hit.m_priRay);
                        if (nh.m_iTriangleIndex >= 0) {
                            nh.m_pNode = pBvhNode;
                            queue.push(nh);
                        }
                    }
                    else {
                        if (pBvhNode->m_pLeft != nullptr) {
                            if (float t = pBvhNode->m_pLeft->intersect(_hit.m_priRay); t >= 0) {
                                boxHits++;
                                MeshIntersect nh;
                                nh.m_fPositionOnRay = t;
                                nh.m_pNode = pBvhNode->m_pLeft;
                                queue.push(nh);
                            }
                        }

                        if (pBvhNode->m_pRight != nullptr) {
                            if (float t = pBvhNode->m_pRight->intersect(_hit.m_priRay); t >= 0) {
                                boxHits++;
                                MeshIntersect nh;
                                nh.m_fPositionOnRay = t;
                                nh.m_pNode = pBvhNode->m_pRight;
                                queue.push(nh);
                            }
                        }
                    }
                }
            }
            

            if (triHit == true) {
                _hit.m_uv = triHit.m_uv;
                _hit.m_iTriangleIndex = triHit.m_iTriangleIndex;
                _hit.m_fPositionOnRay = triHit.m_fPositionOnRay;
                _hit.m_uBoxHits = boxHits;
                _hit.m_uPrimitiveHits = objectHits;
                return true;
            }
            
            return false;
        }

        /* Completes the node intersect properties. */
        virtual BASE::Intersect &intersect(BASE::Intersect &_hit) const override {
            const auto &t = m_triangles[_hit.m_iTriangleIndex];
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
            std::vector<const MeshTriangle*> trianglePtrs = getTrianglePtrs();
            m_pBvhRoot = BASE::buildBvhRoot(trianglePtrs,
                                            [&](){
                                                 m_memory.push_back(std::make_unique<BASE::BvhNode<MeshTriangle>>());
                                                 return m_memory.back().get();
                                            });
        }

     protected:
        // returns index of triangle in mesh
        int32_t getIndex(const MeshTriangle *_pTriangle) const {
            return (int32_t)(_pTriangle - m_triangles.data());
        }
        
        MeshIntersect getMeshIntersect(const MeshTriangle *_pTriangle, const CORE::Ray &_ray) const {
            const auto &v0 = m_vertices[_pTriangle->m_v[0]];
            const auto &v1 = m_vertices[_pTriangle->m_v[1]];
            const auto &v2 = m_vertices[_pTriangle->m_v[2]];

            MeshIntersect mi = triangleIntersect(_ray, v0.m_v, v1.m_v, v2.m_v);
            if (mi.m_fPositionOnRay >= 0) {
                mi.m_iTriangleIndex = getIndex(_pTriangle);
            }
            
            return mi;
        }
        
        // get list of triangles (raw pointers)
        std::vector<const MeshTriangle*> getTrianglePtrs() const {
            std::vector<const MeshTriangle*> trianglePtrs;
            trianglePtrs.reserve(m_triangles.size());
            
            for (auto &t : m_triangles) {
                trianglePtrs.push_back(&t);
            }
            
            return trianglePtrs;
        }

     private:
        std::vector<MeshVertex> m_vertices;
        std::vector<MeshTriangle> m_triangles;
        CORE::Bounds m_bounds;
        const BASE::Material *m_pMaterial;
        bool m_bBoundsInit;
        bool m_bUseVertexNormals;
        BASE::BvhNode<MeshTriangle> *m_pBvhRoot;
        std::vector<std::unique_ptr<BASE::BvhNode<MeshTriangle>>> m_memory;
    };


    /* Sphere built from triangles */
    class SphereMesh    : public Mesh
    {
     public:
        SphereMesh(int _iSlices, int _iDivs, float _fRadius, const BASE::Material *_pMaterial)
            :Mesh(_pMaterial)
        {
            // create vertices
            std::vector<MeshVertex> vertices;
            vertices.reserve(((size_t)_iSlices+1) * ((size_t)_iDivs+1));
            
            for (int d = 0; d <= _iDivs; d++) {
                float angle = pif / _iDivs * d;
                float r = _fRadius * sin(angle);
                float y = _fRadius * cos(angle);
                
                for (int s = 0; s <= _iSlices; s++) {
                    float x = r * cos(pif*2 / _iSlices * s);
                    float z = r * sin(pif*2 / _iSlices * s);
                    
                    auto v = MeshVertex();
                    v.m_v = CORE::Vec(x, y, z);
                    v.m_uv = getSphericalUv(v.m_v.normalized());
                    vertices.push_back(v);
                }
            }

            // create triangles
            std::vector<MeshTriangle> triangles;
            triangles.reserve((size_t)_iSlices * (size_t)_iDivs * 2);

            for (int d = 0; d < _iDivs; d++) {
                for (int s = 0; s < _iSlices; s++) {
                    int i = d * (_iSlices+1) + s;
                    
                    if (d > 0) {
                        auto t = MeshTriangle();
                        t.m_v[0] = (uint32_t)i;
                        t.m_v[1] = (uint32_t)i+1;
                        t.m_v[2] = (uint32_t)i+_iSlices+2;
                        triangles.push_back(t);
                    }
                    
                    if (d < _iDivs-1) {
                        auto t = MeshTriangle();
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

