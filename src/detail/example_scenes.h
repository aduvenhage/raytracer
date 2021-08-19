#ifndef DETAIL_EXAMPLE_SCENES_H
#define DETAIL_EXAMPLE_SCENES_H


#include "core/color.h"
#include "core/vec3.h"
#include "base/loader.h"
#include "base/scene.h"
#include "simple_camera.h"
#include "basic_materials.h"
#include "scatter_materials.h"
#include "marched_bubbles.h"
#include "marched_mandle.h"
#include "marched_materials.h"
#include "marched_sphere.h"
#include "marched_blob.h"
#include "simple_scene.h"
#include "sphere.h"
#include "mesh.h"
#include "plane.h"

#include <memory>


namespace DETAIL
{
    class LoaderMandleBulb  : public BASE::Loader
    {
     public:
        virtual std::string &name() const override {
            static std::string name = "mandlebulb";
            return name;
        }
        
        virtual std::string &description() const override {
            static std::string desc = "Close up of mandle bulb side with glow material";
            return desc;
        }

        virtual std::unique_ptr<BASE::Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(CORE::Color(0.5, 0.5, 0.5));
            auto pGlow = BASE::createMaterial<Glow>(pScene);
            auto pLightWhite = BASE::createMaterial<Light>(pScene, CORE::Color(20.0, 20.0, 20.0));

            BASE::createPrimitiveInstance<Sphere>(pScene, axisTranslation(CORE::Vec(0, 200, 200)), 30, pLightWhite);
            BASE::createPrimitiveInstance<MarchedMandle>(pScene, axisEulerZYX(0, 0, 0, CORE::Vec(0, 0, 0), 40.0), pGlow);
            
            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<BASE::Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(CORE::Vec(50, 0, 10), CORE::Vec(0, 1, 0), CORE::Vec(0, 0, 20), deg2rad(60), 0.5, 8);
        }
    };


    class LoaderRaymarchingBlobs  : public BASE::Loader
    {
     public:
        virtual std::string &name() const override {
            static std::string name = "raymarching_blobs";
            return name;
        }
        
        virtual std::string &description() const override {
            static std::string desc = "Mandlebulb, blobs and bubbles";
            return desc;
        }

        virtual std::unique_ptr<BASE::Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(CORE::Color(0.2, 0.2, 0.2));
            auto pDiffuseFloor = BASE::createMaterial<DiffuseCheckered>(pScene, CORE::Color(1.0, 1.0, 1.0), CORE::Color(1.0, 0.4, 0.2), 2);
            auto pGlass = BASE::createMaterial<Glass>(pScene, CORE::Color(0.95, 0.95, 0.95), 0.01, 1.8);
            auto pMirror = BASE::createMaterial<Metal>(pScene, CORE::Color(0.95, 0.95, 0.95), 0.02);
            auto pGlow = BASE::createMaterial<Glow>(pScene);
            auto pLightWhite = BASE::createMaterial<Light>(pScene, CORE::Color(30.0, 30.0, 30.0));

            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisIdentity(), 500, pDiffuseFloor);
            BASE::createPrimitiveInstance<Rectangle>(pScene, CORE::axisTranslation(CORE::Vec(0, 1, 0)), 200, 200, pMirror);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisTranslation(CORE::Vec(0, 200, 100)), 30, pLightWhite);
            BASE::createPrimitiveInstance<MarchedMandle>(pScene, CORE::axisEulerZYX(0, 1, 0, CORE::Vec(-50, 45, 50), 40.0), pGlow);
            BASE::createPrimitiveInstance<MarchedSphere>(pScene, CORE::axisEulerZYX(0, 1, 0, CORE::Vec(50, 45, 50), 40.0), 2.0f, pGlass);
            BASE::createPrimitiveInstance<MarchedBubbles>(pScene, CORE::axisEulerZYX(0, 1, 0, CORE::Vec(0, 45, -50), 40.0), 2.0f, pGlass);
            
            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<BASE::Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(CORE::Vec(0, 50, 220), CORE::Vec(0, 1, 0), CORE::Vec(0, 5, 0), deg2rad(60), 2.0, 200);
        }
    };


    class LoaderRaymarchingSpheres  : public BASE::Loader
    {
     public:
        virtual std::string &name() const override {
            static std::string name = "raymarching_spheres";
            return name;
        }
        
        virtual std::string &description() const override {
            static std::string desc = "Two marched spheres";
            return desc;
        }

        virtual std::unique_ptr<BASE::Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(CORE::Color(0.2, 0.2, 0.2));
            auto pDiffuseFloor = BASE::createMaterial<DiffuseCheckered>(pScene, CORE::Color(1.0, 1.0, 1.0), CORE::Color(1.0, 0.4, 0.2), 2);
            auto pGlass = BASE::createMaterial<Glass>(pScene, CORE::Color(0.95, 0.95, 0.95), 0.01, 1.8);
            auto pMirror = BASE::createMaterial<Metal>(pScene, CORE::Color(0.95, 0.95, 0.95), 0.02);
            auto pLightWhite = BASE::createMaterial<Light>(pScene, CORE::Color(30.0, 30.0, 30.0));

            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisIdentity(), 500, pDiffuseFloor);
            BASE::createPrimitiveInstance<Rectangle>(pScene, CORE::axisTranslation(CORE::Vec(0, 1, 0)), 200, 200, pMirror);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisTranslation(CORE::Vec(0, 200, 100)), 30, pLightWhite);
            BASE::createPrimitiveInstance<MarchedSphere>(pScene, CORE::axisEulerZYX(0, 1, 0, CORE::Vec(-50, 45, 50), 40.0), 2.0f, pGlass);
            BASE::createPrimitiveInstance<MarchedSphere>(pScene, CORE::axisEulerZYX(0, 1, 0, CORE::Vec(50, 45, 50), 40.0), 2.0f, pGlass);
            
            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<BASE::Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(CORE::Vec(0, 50, 220), CORE::Vec(0, 1, 0), CORE::Vec(0, 5, 0), deg2rad(60), 2.0, 200);
        }
    };


    class LoaderSubsurface  : public BASE::Loader
    {
     public:
        virtual std::string &name() const override {
            static std::string name = "raymarching_subsurface";
            return name;
        }
        
        virtual std::string &description() const override {
            static std::string desc = "Raymarched blobs with sub-surface scattering.";
            return desc;
        }

        virtual std::unique_ptr<BASE::Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(CORE::Color(0.4, 0.4, 0.4));
            auto pDiffuseFloor = BASE::createMaterial<DiffuseCheckered>(pScene, CORE::Color(1.0, 1.0, 1.0), CORE::Color(1.0, 0.4, 0.2), 2);
            auto pGlassGreen = BASE::createMaterial<Glass>(pScene, CORE::Color(0.4f, 0.9f, 0.6f), 0.05, 1.2);
            auto pGlassGreenSs = BASE::createMaterial<GlassScatter>(pScene, CORE::Color(0.4f, 0.9f, 0.6f), 0.05, 1.2);
            auto pMirror = BASE::createMaterial<Metal>(pScene, CORE::Color(0.95, 0.95, 0.95), 0.02);
            auto pLightWhite = BASE::createMaterial<Light>(pScene, CORE::Color(30.0, 30.0, 30.0));

            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisIdentity(), 500, pDiffuseFloor);
            BASE::createPrimitiveInstance<Rectangle>(pScene, CORE::axisTranslation(CORE::Vec(0, 1, 0)), 200, 200, pMirror);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisTranslation(CORE::Vec(0, 100, -50)), 30, pLightWhite);
            BASE::createPrimitiveInstance<MarchedBlob>(pScene, CORE::axisEulerZYX(0, 1, 0, CORE::Vec(-45, 45, 50), 40.0), 2.0f, pGlassGreen, 0.08);
            BASE::createPrimitiveInstance<MarchedBlob>(pScene, CORE::axisEulerZYX(0, 1, 0, CORE::Vec(45, 45, 50), 40.0), 2.0f, pGlassGreenSs, 0.08);
            
            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<BASE::Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(CORE::Vec(0, 50, 220), CORE::Vec(0, 1, 0), CORE::Vec(0, 5, 0), deg2rad(60), 2.0, 200);
        }
    };
    class LoaderManySpheres  : public BASE::Loader
    {
     public:
        virtual std::string &name() const override {
            static std::string name = "many_spheres";
            return name;
        }
        
        virtual std::string &description() const override {
            static std::string desc = "Many spheres (BVH Test)";
            return desc;
        }

        virtual std::unique_ptr<BASE::Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(CORE::Color(0.2, 0.2, 0.2));
            auto pDiffuseRed = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.9f, 0.1f, 0.1f));
            auto pDiffuseGreen = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.1f, 0.9f, 0.1f));
            auto pDiffuseBlue = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.1f, 0.1f, 0.9f));
            
            auto pSphere1 = BASE::createPrimitive<Sphere>(pScene, 4, pDiffuseRed);
            auto pSphere2 = BASE::createPrimitive<Sphere>(pScene, 4, pDiffuseGreen);
            auto pSphere3 = BASE::createPrimitive<Sphere>(pScene, 4, pDiffuseBlue);
            auto shapes = std::vector{pSphere1, pSphere2, pSphere3};

            auto pLightWhite = BASE::createMaterial<Light>(pScene, CORE::Color(10.0f, 10.0f, 10.0f));
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisTranslation(CORE::Vec(0, 200, 100)), 30, pLightWhite);
            
            int n = 200;
            for (int i = 0; i < n; i++) {
                
                float x = 100 * sin((float)i / n * CORE::pi * 2);
                float y = 20 * (cos((float)i / n * CORE::pi * 16) + 1);
                float z = 100 * cos((float)i / n * CORE::pi * 2);

                BASE::createPrimitiveInstance(pScene, CORE::axisEulerZYX(0, 0, 0, CORE::Vec(x, y, z)), shapes[i % shapes.size()]);
            }

            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<BASE::Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(CORE::Vec(0, 50, 220), CORE::Vec(0, 1, 0), CORE::Vec(0, 5, 0), deg2rad(60), 2.0, 150);
        }
    };


    class LoaderManySpheresTri  : public BASE::Loader
    {
     public:
        virtual std::string &name() const override {
            static std::string name = "many_spheres_tri";
            return name;
        }
        
        virtual std::string &description() const override {
            static std::string desc = "Many spheres (BVH Test) built with triangles";
            return desc;
        }

        virtual std::unique_ptr<BASE::Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(CORE::Color(0.2, 0.2, 0.2));
            auto pDiffuseRed = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.9f, 0.1f, 0.1f));
            auto pDiffuseGreen = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.1f, 0.9f, 0.1f));
            auto pDiffuseBlue = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.1f, 0.1f, 0.9f));
            
            auto pMesh1 = BASE::createPrimitive<SphereMesh>(pScene, 16, 16, 4, pDiffuseRed);
            auto pMesh2 = BASE::createPrimitive<SphereMesh>(pScene, 16, 16, 4, pDiffuseGreen);
            auto pMesh3 = BASE::createPrimitive<SphereMesh>(pScene, 16, 16, 4, pDiffuseBlue);
            auto shapes = std::vector{pMesh1, pMesh2, pMesh3};

            auto pLightWhite = BASE::createMaterial<Light>(pScene, CORE::Color(10.0f, 10.0f, 10.0f));
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisTranslation(CORE::Vec(0, 200, 100)), 30, pLightWhite);
            
            int n = 200;
            for (int i = 0; i < n; i++) {
                
                float x = 100 * sin((float)i / n * CORE::pi * 2);
                float y = 20 * (cos((float)i / n * CORE::pi * 16) + 1);
                float z = 100 * cos((float)i / n * CORE::pi * 2);

                BASE::createPrimitiveInstance(pScene, CORE::axisEulerZYX(0, 0, 0, CORE::Vec(x, y, z)), shapes[i % shapes.size()]);
            }

            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<BASE::Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(CORE::Vec(0, 50, 220), CORE::Vec(0, 1, 0), CORE::Vec(0, 5, 0), deg2rad(60), 2.0, 150);
        }
    };


    class LoaderSceneStackedSpheres  : public BASE::Loader
    {
     public:
        virtual std::string &name() const override {
            static std::string name = "stacked_spheres";
            return name;
        }
        
        virtual std::string &description() const override {
            static std::string desc = "Many spheres stacked in a cube";
            return desc;
        }

        virtual std::unique_ptr<BASE::Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(CORE::Color(0.2, 0.2, 0.2));
            auto pDiffuseFloor = BASE::createMaterial<DiffuseCheckered>(pScene, CORE::Color(0.1, 1.0, 0.1), CORE::Color(0.1, 0.1, 1.0), 2);
            auto pGlass = BASE::createMaterial<Glass>(pScene, CORE::Color(0.99, 0.99, 0.99), 0.01, 1.8);
            auto pLight = BASE::createMaterial<Light>(pScene, CORE::Color(10.0f, 10.0f, 10.0f));

            auto pSphere = BASE::createPrimitive<Sphere>(pScene, 10, pGlass);

            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisTranslation(CORE::Vec(0, 500, 0)), 100, pLight);
            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisTranslation(CORE::Vec(0, -100, 0)), 500, pDiffuseFloor);

            for (int x = -2; x <= 2; x++) {
                for (int y = -2; y <= 2; y++) {
                    for (int z = -2; z <= 2; z++) {
                        BASE::createPrimitiveInstance(pScene, CORE::axisTranslation(CORE::Vec(x * 20, y * 20, z * 20)), pSphere);
                    }
                }
            }

            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<BASE::Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(CORE::Vec(100, 80, 100), CORE::Vec(0, 1, 0), CORE::Vec(0, 5, 0), deg2rad(60), 5.0, 100);
        }
    };


    auto getSceneList() {
        return std::vector<std::shared_ptr<BASE::Loader>>{
            std::make_shared<LoaderMandleBulb>(),
            std::make_shared<LoaderRaymarchingBlobs>(),
            std::make_shared<LoaderRaymarchingSpheres>(),
            std::make_shared<LoaderManySpheres>(),
            std::make_shared<LoaderManySpheresTri>(),
            std::make_shared<LoaderSceneStackedSpheres>(),
            std::make_shared<LoaderSubsurface>()
        };
    }

    
};  // namespace DETAIL


#endif  // #ifndef DETAIL_EXAMPLE_SCENES_H

