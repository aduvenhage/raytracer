#pragma once

#include "core/bvh.h"
#include "core/constants.h"
#include "core/vec3.h"
#include "core/uv.h"
#include "base/primitive.h"
#include "base/material.h"
#include "mesh.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags


namespace DETAIL
{
    /* Mesh defined by vertices, triangle indices and a material */
    class AssimpMesh : public Mesh
    {
     public:
         AssimpMesh(const char* _pszFilePath, const BASE::Material* _pMaterial)
             :Mesh(_pMaterial)
         {
             // load model
             Assimp::Importer importer;
             const aiScene* pScene = importer.ReadFile(_pszFilePath, aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_Triangulate);

             if (pScene->HasMeshes() == true) {
                 // load first mesh
                 const aiMesh *pMesh = pScene->mMeshes[0];
                 std::vector<Mesh::Vertex> vertices;
                 vertices.reserve(pMesh->mNumVertices);

                 // load mesh vertices
                 for (int i = 0; i < (int)pMesh->mNumVertices; i++) {
                     Mesh::Vertex vertex;
                     if (pMesh->HasPositions() == true) {
                         const aiVector3D &aiVertex = pMesh->mVertices[i];
                         vertex.m_v = CORE::Vec(aiVertex.x, aiVertex.y, aiVertex.z);
                     }

                     if (pMesh->HasTextureCoords(0) == true) {
                         const aiVector3D &aiUv = pMesh->mTextureCoords[0][i];
                         vertex.m_uv = CORE::Uv(aiUv.x, aiUv.y);
                     }

                     if (pMesh->HasNormals() == true) {
                         const aiVector3D &aiNormal = pMesh->mNormals[i];
                         vertex.m_normal = CORE::Vec(aiNormal.x, aiNormal.y, aiNormal.z);
                     }

                     vertices.push_back(vertex);
                 }

                 setVertices(vertices);

                 // load indices
                 std::vector<Mesh::Triangle> triangles;
                 triangles.reserve(pMesh->mNumFaces);

                 for (int i = 0; i < (int)pMesh->mNumFaces; i++) {
                     Mesh::Triangle triangle;
                     const aiFace &face = pMesh->mFaces[i];
                     triangle.m_v[0] = face.mIndices[0];
                     triangle.m_v[1] = face.mIndices[1];
                     triangle.m_v[2] = face.mIndices[2];

                     // TODO: is this the correct way of finding the triangle normal (when using aiProcess_GenNormals)
                     triangle.m_normal = vertices[face.mIndices[0]].m_normal;

                     triangles.push_back(triangle);
                 }

                 setTriangles(triangles);
             }

             // complete mesh
             //buildVertexNormals();
             buildBounds();
             buildBvh();
         }

    };

};  // namespace DETAIL

