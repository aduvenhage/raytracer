#ifndef LIBS_HEADER_EXAMPLE_SCENES_H
#define LIBS_HEADER_EXAMPLE_SCENES_H


#include "simple_camera.h"
#include "color.h"
#include "default_materials.h"
#include "loader.h"
#include "marched_bubbles.h"
#include "marched_mandle.h"
#include "marched_materials.h"
#include "marched_sphere.h"
#include "scene.h"
#include "simple_scene.h"
#include "sphere.h"
#include "plane.h"
#include "vec3.h"

#include <memory>


namespace LNF
{
    // scene -- mandlebulb
    class LoaderScene0  : public Loader
    {
     public:
        virtual std::unique_ptr<Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(Color(0.5, 0.5, 0.5));
            auto pAO = createMaterial<FakeAmbientOcclusion>(pScene);
            auto pGlow = createMaterial<Glow>(pScene);
            auto pLightWhite = createMaterial<Light>(pScene, Color(20.0, 20.0, 20.0));

            createPrimitiveInstance<Sphere>(pScene, axisTranslation(Vec(0, 200, 200)), 30, pLightWhite);
            createPrimitiveInstance<MarchedMandle>(pScene, axisEulerZYX(0, 0, 0, Vec(0, 0, 0), 40.0), pGlow);
            
            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(Vec(50, 0, 10), Vec(0, 1, 0), Vec(0, 0, 20), deg2rad(60), 0.5, 8);
        }
    };


    // scene -- raymarching
    class LoaderScene1  : public Loader
    {
     public:
        virtual std::unique_ptr<Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(Color(0.2, 0.2, 0.2));
            auto pDiffuseFloor = createMaterial<DiffuseCheckered>(pScene, Color(1.0, 1.0, 1.0), Color(1.0, 0.4, 0.2), 2);
            //auto pDiffuseFog = createMaterial<Diffuse>(_pScene.get(), Color(0.9, 0.9, 0.9));
            auto pGlass = createMaterial<Glass>(pScene, Color(0.95, 0.95, 0.95), 0.01, 1.8);
            auto pMirror = createMaterial<Metal>(pScene, Color(0.95, 0.95, 0.95), 0.02);
            auto pAO = createMaterial<FakeAmbientOcclusion>(pScene);
            auto pMetalIt = createMaterial<MetalIterations>(pScene);
            auto pGlow = createMaterial<Glow>(pScene);
            auto pLightWhite = createMaterial<Light>(pScene, Color(30.0, 30.0, 30.0));

            createPrimitiveInstance<Disc>(pScene, axisIdentity(), 500, pDiffuseFloor);
            createPrimitiveInstance<Rectangle>(pScene, axisTranslation(Vec(0, 1, 0)), 200, 200, pMirror);
            //createPrimitiveInstance<SmokeBox>(_pScene.get(), axisIdentity(), 400, pDiffuseFog, 400);
            createPrimitiveInstance<Sphere>(pScene, axisTranslation(Vec(0, 200, 100)), 30, pLightWhite);
            createPrimitiveInstance<MarchedMandle>(pScene, axisEulerZYX(0, 1, 0, Vec(-50, 45, 50), 40.0), pGlow);
            createPrimitiveInstance<MarchedSphere>(pScene, axisEulerZYX(0, 1, 0, Vec(50, 45, 50), 40.0), 2.0f, pGlass, 0.04f);
            createPrimitiveInstance<MarchedBubbles>(pScene, axisEulerZYX(0, 1, 0, Vec(0, 45, -50), 40.0), 2.0f, pGlass);
            
            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(Vec(0, 50, 220), Vec(0, 1, 0), Vec(0, 5, 0), deg2rad(60), 2.0, 200);
        }
    };


    // scene -- many spheres
    class LoaderScene2  : public Loader
    {
     public:
        virtual std::unique_ptr<Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(Color(0.2, 0.2, 0.2));
            auto pDiffuseRed = createMaterial<Diffuse>(pScene, Color(0.9f, 0.1f, 0.1f));
            auto pDiffuseGreen = createMaterial<Diffuse>(pScene, Color(0.1f, 0.9f, 0.1f));
            auto pDiffuseBlue = createMaterial<Diffuse>(pScene, Color(0.1f, 0.1f, 0.9f));
            
            //auto pMesh1 = createPrimitive<SphereMesh>(pScene, 16, 16, 4, pDiffuseRed);
            //auto pMesh2 = createPrimitive<SphereMesh>(pScene, 16, 16, 4, pDiffuseGreen);
            //auto pMesh3 = createPrimitive<SphereMesh>(pScene, 16, 16, 4, pDiffuseBlue);
            //auto shapes = std::vector{pMesh1, pMesh2, pMesh3};

            auto pSphere1 = createPrimitive<Sphere>(pScene, 4, pDiffuseRed);
            auto pSphere2 = createPrimitive<Sphere>(pScene, 4, pDiffuseGreen);
            auto pSphere3 = createPrimitive<Sphere>(pScene, 4, pDiffuseBlue);
            auto shapes = std::vector{pSphere1, pSphere2, pSphere3};

            auto pLightWhite = createMaterial<Light>(pScene, Color(10.0f, 10.0f, 10.0f));
            createPrimitiveInstance<Sphere>(pScene, axisTranslation(Vec(0, 200, 100)), 30, pLightWhite);
            
            int n = 200;
            for (int i = 0; i < n; i++) {
                
                float x = 100 * sin((float)i / n * LNF::pi * 2);
                float y = 20 * (cos((float)i / n * LNF::pi * 16) + 1);
                float z = 100 * cos((float)i / n * LNF::pi * 2);

                createPrimitiveInstance(pScene, axisEulerZYX(0, 0, 0, Vec(x, y, z)), shapes[i % shapes.size()]);
            }

            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(Vec(0, 50, 220), Vec(0, 1, 0), Vec(0, 5, 0), deg2rad(60), 2.0, 150);
        }
    };


    // scene -- many spheres (stacked in a cube)
    class LoaderScene3  : public Loader
    {
     public:
        virtual std::unique_ptr<Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(Color(0.2, 0.2, 0.2));
            auto pDiffuseFloor = createMaterial<DiffuseCheckered>(pScene, Color(0.1, 1.0, 0.1), Color(0.1, 0.1, 1.0), 2);
            auto pGlass = createMaterial<Glass>(pScene, Color(0.99, 0.99, 0.99), 0.01, 1.8);
            auto pLight = createMaterial<Light>(pScene, Color(10.0f, 10.0f, 10.0f));

            auto pSphere = createPrimitive<Sphere>(pScene, 10, pGlass);

            createPrimitiveInstance<Sphere>(pScene, axisTranslation(Vec(0, 500, 0)), 100, pLight);
            createPrimitiveInstance<Disc>(pScene, axisTranslation(Vec(0, -100, 0)), 500, pDiffuseFloor);

            for (int x = -2; x <= 2; x++) {
                for (int y = -2; y <= 2; y++) {
                    for (int z = -2; z <= 2; z++) {
                        createPrimitiveInstance(pScene, axisTranslation(Vec(x * 20, y * 20, z * 20)), pSphere);
                    }
                }
            }

            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(Vec(100, 80, 100), Vec(0, 1, 0), Vec(0, 5, 0), deg2rad(60), 5.0, 100);
        }
    };

    
};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_EXAMPLE_SCENES_H

