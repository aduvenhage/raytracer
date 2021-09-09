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
#include "marched_torus.h"
#include "special_materials.h"
#include "simple_scene.h"
#include "smoke_box.h"
#include "sphere.h"
#include "mesh.h"
#include "plane.h"
#include "box.h"

#include <memory>


namespace DETAIL
{
    class LoaderDefaultScene  : public BASE::Loader
    {
     public:
        virtual std::string &name() const override {
            static std::string name = "default_scene";
            return name;
        }
        
        virtual std::string &description() const override {
            static std::string desc = "Default scene with many different primitives and materials";
            return desc;
        }

        virtual std::unique_ptr<BASE::Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(CORE::Color(0.2, 0.2, 0.3));
            auto pDiffuseFloor = BASE::createMaterial<DiffuseCheckered>(pScene, CORE::Color(0.8, 0.8, 0.1), CORE::Color(0.8, 0.1, 0.1), 2);
            auto pLight = BASE::createMaterial<Light>(pScene, CORE::Color(30.0, 30.0, 30.0));
            auto pDiffuseCheck = BASE::createMaterial<DiffuseCheckered>(pScene, CORE::Color(0.1, 0.1, 0.8), CORE::Color(0.1, 0.8, 0.8), 8);
            auto pDiffuseBlue = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.2, 0.2, 0.6));
            auto pDiffuseWhite = BASE::createMaterial<DiffuseCheckered>(pScene, CORE::Color(0.3, 0.3, 0.3), CORE::Color(0.9, 0.9, 0.9), 8);
            auto pMirror = BASE::createMaterial<Metal>(pScene, CORE::Color(0.95, 0.95, 0.95), 0.01);
            auto pMetal = BASE::createMaterial<Metal>(pScene, CORE::Color(0.90, 0.90, 0.90), 0.1);
            auto pGlass = BASE::createMaterial<Glass>(pScene, CORE::Color(0.95, 0.95, 0.95), 0.01, 1.8);
            
            auto pMeshSphere = BASE::createPrimitive<SphereMesh>(pScene, 16, 16, 10, pDiffuseWhite);

            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisIdentity(), 500, pDiffuseFloor);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisTranslation(CORE::Vec(0, 100, 0)), 40, pLight);
            BASE::createPrimitiveInstance<Box>(pScene, CORE::axisEulerZYX(0, 0.8, 0, CORE::Vec(20, 30, 0)), CORE::Vec(20, 40, 20), pMirror);
            BASE::createPrimitiveInstance<Box>(pScene, CORE::axisEulerZYX(0, -0.8, 0, CORE::Vec(-35, 20, 30)), 15, pMetal);
            BASE::createPrimitiveInstance<MarchedTorus>(pScene, CORE::axisEulerZYX(0, 0, 0, CORE::Vec(-20, 20, 0)), 15.0f, 7.0f, pDiffuseCheck);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisEulerZYX(0, 0, 0, CORE::Vec(0, 20, -40)), 15.0f, pDiffuseBlue);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisEulerZYX(0, 0, 0, CORE::Vec(0, 17, 25)), 15.0f, pGlass);
            BASE::createPrimitiveInstance(pScene, CORE::axisEulerZYX(0, 1.4, 0, CORE::Vec(-20, 7, 40)), pMeshSphere);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisEulerZYX(0, 1.4, 0, CORE::Vec(20, 7, 40)), 10.0f, pDiffuseWhite);

            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<BASE::Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(CORE::Vec(0, 60, 100), CORE::Vec(0, 1, 0), CORE::Vec(0, 0, 0), deg2rad(60), 1.4, 100);
        }
    };


    class LoaderGlassSphereScene  : public BASE::Loader
    {
     public:
        virtual std::string &name() const override {
            static std::string name = "glass_sphere";
            return name;
        }
        
        virtual std::string &description() const override {
            static std::string desc = "One big glass sphere";
            return desc;
        }

        virtual std::unique_ptr<BASE::Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(CORE::Color(0.2, 0.2, 0.3));
            auto pDiffuseFloor = BASE::createMaterial<DiffuseCheckered>(pScene, CORE::Color(0.8, 0.8, 0.1), CORE::Color(0.8, 0.1, 0.1), 2);
            auto pLight = BASE::createMaterial<Light>(pScene, CORE::Color(70.0, 70.0, 70.0));
            auto pGlass = BASE::createMaterial<Glass>(pScene, CORE::Color(0.95, 0.95, 0.95), 0.01, 1.8);
            auto pMetal = BASE::createMaterial<Metal>(pScene, CORE::Color(0.5, 0.5, 0.5), 0.05);
            
            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisTranslation(CORE::Vec(0, 0, 0)), 500, pDiffuseFloor);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisTranslation(CORE::Vec(0, 100, 0)), 20, pLight);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisTranslation(CORE::Vec(-30, 20, 0)), 20.0f, pGlass);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisTranslation(CORE::Vec(30, 20, 0)), 20.0f, pMetal);

            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<BASE::Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(CORE::Vec(0, 40, 100), CORE::Vec(0, 1, 0), CORE::Vec(0, 20, 0), deg2rad(60), 1.4, 120);
        }
    };


    class LoaderBulbFieldScene  : public BASE::Loader
    {
     public:
        virtual std::string &name() const override {
            static std::string name = "bulb_field";
            return name;
        }
        
        virtual std::string &description() const override {
            static std::string desc = "Default scene with many different primitives and materials";
            return desc;
        }

        virtual std::unique_ptr<BASE::Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(CORE::Color(0.4, 0.4, 0.5));
            auto pDiffuseFloor = BASE::createMaterial<DiffuseCheckered>(pScene, CORE::Color(0.8, 0.8, 0.1), CORE::Color(0.8, 0.1, 0.1), 2);
            auto pLight = BASE::createMaterial<Light>(pScene, CORE::Color(50.0, 50.0, 50.0));
            auto pDiffuseWhite = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.9, 0.9, 0.9));
            auto pAO = BASE::createMaterial<FakeAmbientOcclusion>(pScene);
            auto pIterations = BASE::createMaterial<MetalIterations>(pScene);
            auto pGlow = BASE::createMaterial<Glow>(pScene);
            
            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisIdentity(), 500, pDiffuseFloor);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisTranslation(CORE::Vec(0, 100, 0)), 10, pLight);
            BASE::createPrimitiveInstance<MarchedMandle>(pScene, axisEulerZYX(0, 0, 0, CORE::Vec(0, 15, 0), 15.0), pDiffuseWhite);
            BASE::createPrimitiveInstance<MarchedMandle>(pScene, axisEulerZYX(0, 0, 0, CORE::Vec(-30, 15, 0), 15.0), pAO);
            BASE::createPrimitiveInstance<MarchedMandle>(pScene, axisEulerZYX(0, 0, 0, CORE::Vec(30, 15, 0), 15.0), pIterations);
            BASE::createPrimitiveInstance<MarchedMandle>(pScene, axisEulerZYX(0, 0, 0, CORE::Vec(0, 15, 40), 15.0), pGlow);

            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<BASE::Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(CORE::Vec(0, 60, 100), CORE::Vec(0, 1, 0), CORE::Vec(0, 0, 0), deg2rad(60), 0.1, 180);
        }
    };


    class LoaderMandleBulbZoom  : public BASE::Loader
    {
     public:
        virtual std::string &name() const override {
            static std::string name = "mandlebulb_zoom";
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


    class LoaderRaymarchingTorus  : public BASE::Loader
    {
     public:
        virtual std::string &name() const override {
            static std::string name = "raymarching_torus";
            return name;
        }
        
        virtual std::string &description() const override {
            static std::string desc = "Marched shiny torus";
            return desc;
        }

        virtual std::unique_ptr<BASE::Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(CORE::Color(0.2, 0.2, 0.2));
            auto pDiffuse = BASE::createMaterial<DiffuseCheckered>(pScene, CORE::Color(0.8, 0.2, 0.2), CORE::Color(0.9, 0.9, 0.9), 8);
            auto pDiffuseFloor = BASE::createMaterial<DiffuseCheckered>(pScene, CORE::Color(0.2, 0.2, 0.2), CORE::Color(0.8, 0.8, 0.8), 2);
            auto pMirror = BASE::createMaterial<Metal>(pScene, CORE::Color(0.95, 0.95, 0.95), 0.05);
            auto pLight = BASE::createMaterial<Light>(pScene, CORE::Color(50.0, 50.0, 50.0));

            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisIdentity(), 500, pDiffuseFloor);
            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisTranslation(CORE::Vec(0, 1, 0)), 80, pMirror);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisTranslation(CORE::Vec(50, 100, 0)), 20, pLight);
            BASE::createPrimitiveInstance<MarchedTorus>(pScene, CORE::axisTranslation(CORE::Vec(0, 20, 0)), 40.0f, 10.0f, pDiffuse);
            
            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<BASE::Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(CORE::Vec(0, 40, 100), CORE::Vec(0, 1, 0), CORE::Vec(0, 0, 0), deg2rad(60), 2.0, 80);
        }
    };


    class LoaderSubsurfaceSpheres  : public BASE::Loader
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
            auto pGlassGreen = BASE::createMaterial<Glass>(pScene, CORE::Color(0.4f, 0.9f, 0.6f), 0.05, 1.8);
            auto pGlassGreenSs = BASE::createMaterial<GlassScatter>(pScene, CORE::Color(0.4f, 0.9f, 0.6f), 0.05, 1.8);
            auto pMirror = BASE::createMaterial<Metal>(pScene, CORE::Color(0.95, 0.95, 0.95), 0.02);
            auto pLightWhite = BASE::createMaterial<Light>(pScene, CORE::Color(30.0, 30.0, 30.0));

            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisIdentity(), 500, pDiffuseFloor);
            BASE::createPrimitiveInstance<Rectangle>(pScene, CORE::axisTranslation(CORE::Vec(0, 1, 0)), 200, 200, pMirror);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisTranslation(CORE::Vec(0, 100, -50)), 30, pLightWhite);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisEulerZYX(0, 1, 0, CORE::Vec(-45, 45, 50), 20.0), 2.0f, pGlassGreen);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisEulerZYX(0, 1, 0, CORE::Vec(45, 45, 50), 20.0), 2.0f, pGlassGreenSs);
            
            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<BASE::Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(CORE::Vec(0, 50, 220), CORE::Vec(0, 1, 0), CORE::Vec(0, 5, 0), deg2rad(60), 2.0, 200);
        }
    };


    /* TODO: fix ss scattering */
    class LoaderSubsurfaceBlobs  : public BASE::Loader
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
            auto pScene = std::make_unique<SimpleSceneBvh>(CORE::Color(0.1, 0.1, 0.1));
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
            auto pScene = std::make_unique<SimpleSceneBvh>(CORE::Color(0.1, 0.1, 0.1));
            auto pDiffuseRed = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.9f, 0.1f, 0.1f));
            auto pDiffuseGreen = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.1f, 0.9f, 0.1f));
            auto pDiffuseBlue = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.1f, 0.1f, 0.9f));
            
            auto pSphere1 = BASE::createPrimitive<Sphere>(pScene, 4, pDiffuseRed);
            auto pSphere2 = BASE::createPrimitive<Sphere>(pScene, 4, pDiffuseGreen);
            auto pSphere3 = BASE::createPrimitive<Sphere>(pScene, 4, pDiffuseBlue);
            auto shapes = std::vector{pSphere1, pSphere2, pSphere3};

            auto pLightWhite = BASE::createMaterial<Light>(pScene, CORE::Color(20.0f, 20.0f, 20.0f));
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisTranslation(CORE::Vec(0, 200, 100)), 50, pLightWhite);
            
            int n = 200;
            for (int i = 0; i < n; i++) {
                
                float x = 100 * sin((float)i / n * pi * 2);
                float y = 20 * (cos((float)i / n * pi * 16) + 1);
                float z = 100 * cos((float)i / n * pi * 2);

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
                
                float x = 100 * sin((float)i / n * pi * 2);
                float y = 20 * (cos((float)i / n * pi * 16) + 1);
                float z = 100 * cos((float)i / n * pi * 2);

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


    class LoaderFogScene  : public BASE::Loader
    {
     public:
        virtual std::string &name() const override {
            static std::string name = "something_in_fog";
            return name;
        }
        
        virtual std::string &description() const override {
            static std::string desc = "Testing fog materials";
            return desc;
        }

        virtual std::unique_ptr<BASE::Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(CORE::Color(0.3, 0.3, 0.35));
            auto pDiffuseFloor = BASE::createMaterial<DiffuseCheckered>(pScene, CORE::Color(0.8, 0.8, 0.8), CORE::Color(0.4, 0.4, 0.4), 2);
            auto pDiffuseCheck = BASE::createMaterial<DiffuseCheckered>(pScene, CORE::Color(0.8, 0.8, 0.8), CORE::Color(0.4, 0.4, 0.4), 8);
            auto pLight = BASE::createMaterial<Light>(pScene, CORE::Color(60.0, 60.0, 40.0));
            auto pRed = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.95, 0.0, 0.0));
            auto pGreen = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.0, 0.95, 0.0));
            auto pBlue = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.0, 0.0, 0.95));
            auto pFog = BASE::createMaterial<EnvironmentMap>(pScene, CORE::Color(0.9, 0.7, 0.6));

            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisIdentity(), 500, pDiffuseFloor);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisTranslation(CORE::Vec(0, 80, 0)), 10, pLight);
            BASE::createPrimitiveInstance<MarchedTorus>(pScene, CORE::axisEulerZYX(0, 0, 0, CORE::Vec(0, 75, 0)), 15.0f, 7.0f, pDiffuseCheck);
            BASE::createPrimitiveInstance<MarchedTorus>(pScene, CORE::axisEulerZYX(0, 0, 0, CORE::Vec(0, 85, 0)), 15.0f, 7.0f, pDiffuseCheck);

            BASE::createPrimitiveInstance<SmokeBox>(pScene, CORE::axisEulerZYX(0, 0, 0, CORE::Vec(0, 50, 0)), CORE::Vec(400, 100, 400), pFog, 400);
            
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisEulerZYX(0, 0, 0, CORE::Vec(-70, 15, 0)), 15.0f, pRed);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisEulerZYX(0, 0, 0, CORE::Vec(0, 15, 0)), 15.0f, pGreen);
            BASE::createPrimitiveInstance<Sphere>(pScene, CORE::axisEulerZYX(0, 0, 0, CORE::Vec(70, 15, 0)), 15.0f, pBlue);

            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<BASE::Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(CORE::Vec(0, 60, 150), CORE::Vec(0, 1, 0), CORE::Vec(0, 40, 0), deg2rad(60), 0.1, 180);
        }
    };


    class LoaderCornellBox  : public BASE::Loader
    {
     public:
        virtual std::string &name() const override {
            static std::string name = "cornell_box";
            return name;
        }
        
        virtual std::string &description() const override {
            static std::string desc = "Standard cornell box scene";
            return desc;
        }

        virtual std::unique_ptr<BASE::Scene> loadScene() const override {
            auto pScene = std::make_unique<SimpleSceneBvh>(CORE::Color(0.0, 0.0, 0.0));
            
            
            auto pDiffuseGrey = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.7, 0.7, 0.7));
            auto pDiffuseRed = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.7, 0.0, 0.0));
            auto pDiffuseGreen = BASE::createMaterial<Diffuse>(pScene, CORE::Color(0.0, 0.7, 0.0));
            auto pLight = BASE::createMaterial<Light>(pScene, CORE::Color(30.0, 30.0, 30.0));
            
            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisEulerZYX(0, 0, 0, CORE::Vec(0, 0, 0)), 100, pDiffuseGrey);
            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisEulerZYX(0, 0, pi/2, CORE::Vec(0, 50, -50)), 100, pDiffuseGrey);
            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisEulerZYX(0, 0, pi, CORE::Vec(0, 100, 0)), 100, pDiffuseGrey);
            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisEulerZYX(-pi/2, 0, 0, CORE::Vec(-50, 50, 0)), 100, pDiffuseGreen);
            BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisEulerZYX(pi/2, 0, 0, CORE::Vec(50, 50, 0)), 100, pDiffuseRed);
            BASE::createPrimitiveInstance<Box>(pScene, CORE::axisEulerZYX(0, 0.5, 0, CORE::Vec(-15, 25, -15)), CORE::Vec(25, 50, 25), pDiffuseGrey);
            BASE::createPrimitiveInstance<Box>(pScene, CORE::axisEulerZYX(0, -0.5, 0, CORE::Vec(15, 12.5, 10)), CORE::Vec(25, 25, 25), pDiffuseGrey);
            //BASE::createPrimitiveInstance<Disc>(pScene, CORE::axisEulerZYX(pi, 0, 0, CORE::Vec(0, 99.99, 0)), 15, pLight);
            BASE::createPrimitiveInstance<Rectangle>(pScene, CORE::axisEulerZYX(pi, 0, 0, CORE::Vec(0, 99.99, 0)), 15, 15, pLight);
            

            pScene->build();   // build BVH
            return pScene;
        }

        virtual std::unique_ptr<BASE::Camera> loadCamera() const override {
            return std::make_unique<SimpleCamera>(CORE::Vec(0, 50, 120), CORE::Vec(0, 1, 0), CORE::Vec(0, 50, 0), deg2rad(60), 0.1, 120);
        }
    };





    auto getSceneList() {
        return std::vector<std::shared_ptr<BASE::Loader>>{
            std::make_shared<LoaderDefaultScene>(),
            std::make_shared<LoaderGlassSphereScene>(),
            std::make_shared<LoaderMandleBulbZoom>(),
            std::make_shared<LoaderRaymarchingBlobs>(),
            std::make_shared<LoaderRaymarchingSpheres>(),
            std::make_shared<LoaderRaymarchingTorus>(),
            std::make_shared<LoaderManySpheres>(),
            std::make_shared<LoaderManySpheresTri>(),
            std::make_shared<LoaderSceneStackedSpheres>(),
            std::make_shared<LoaderSubsurfaceSpheres>(),
            std::make_shared<LoaderSubsurfaceBlobs>(),
            std::make_shared<LoaderBulbFieldScene>(),
            std::make_shared<LoaderFogScene>(),
            std::make_shared<LoaderCornellBox>(),
        };
    }




    
};  // namespace DETAIL


#endif  // #ifndef DETAIL_EXAMPLE_SCENES_H

