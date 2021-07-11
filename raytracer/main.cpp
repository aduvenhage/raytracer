#include "lnf/box.h"
#include "lnf/bvh.h"
#include "lnf/color.h"
#include "lnf/constants.h"
#include "lnf/default_materials.h"
#include "lnf/marched_materials.h"
#include "lnf/frame.h"
#include "lnf/jobs.h"
#include "lnf/jpeg.h"
#include "lnf/marched_bubbles.h"
#include "lnf/marched_mandle.h"
#include "lnf/marched_sphere.h"
#include "lnf/mesh.h"
#include "lnf/outputimage.h"
#include "lnf/plane.h"
#include "lnf/profile.h"
#include "lnf/ray.h"
#include "lnf/scene.h"
#include "lnf/smoke_box.h"
#include "lnf/sphere.h"
#include "lnf/trace.h"
#include "lnf/uv.h"
#include "lnf/vec3.h"
#include "lnf/viewport.h"
#include "lnf/queue.h"

#include <assert.h>
#include <vector>
#include <memory>
#include <random>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <iostream>
#include <string>

#include <QtWidgets>



using namespace LNF;


/*
 TODO:
 - add camera to load scene function.
 - add cornell box scene
 
 */


// simple scene with a linear search for object hits
class SimpleScene   : public Scene
{
 public:
    SimpleScene()
    {}
        
    /*
       Checks for an intersect with a scene object.
       Could be accessed by multiple worker threads concurrently.
     */
    virtual bool hit(Intersect &_hit) const override {
        for (const auto &pObj : m_objects) {
            Intersect nh(_hit);
            if ( (pObj->hit(nh) == true) &&
                 ( (_hit == false) || (nh.m_fPositionOnRay < _hit.m_fPositionOnRay)) )
            {
                _hit = nh;
            }
        }
        
        return _hit;
    }
    
    /*
     Checks for the background color (miss handler).
     Could be accessed by multiple worker threads concurrently.
     */
    virtual Color backgroundColor() const override {
        return Color(0.2f, 0.2f, 0.2f);
    }

    /*
     Add a new resource (material, primitive, instance) to the scene.
     May not be safe to call while worker threads are calling 'hit'/
    */
    virtual Resource *addResource(std::unique_ptr<Resource> &&_pResource) override {
        m_resources.push_back(std::move(_pResource));
        return m_resources.back().get();
    }

    /*
     Add a new primitive instance to the scene.
     May not be safe to call while worker threads are calling 'hit'/
     */
    virtual PrimitiveInstance *addPrimitiveInstance(std::unique_ptr<PrimitiveInstance> &&_pInstance) override {
        m_objects.push_back(std::move(_pInstance));
        return m_objects.back().get();
    }
    
 protected:
    std::vector<std::unique_ptr<Resource>>           m_resources;
    std::vector<std::unique_ptr<PrimitiveInstance>>  m_objects;
};


// simple scene using a BVH for optimising hits
class SimpleSceneBvh   : public SimpleScene
{
 public:
    SimpleSceneBvh()
    {}
        
    // Checks for an intersect with a scene object (could be accessed by multiple worker threads concurrently).
    virtual bool hit(Intersect &_hit) const override {
        return checkBvhHitI(_hit);
        //return checkBvhHitR(_hit, m_root);
    }

    // Build acceleration structures
    void build() {
        std::vector<const PrimitiveInstance*> rawObjects(m_objects.size(), nullptr);
        for (size_t i = 0; i < m_objects.size(); i++) {
            rawObjects[i] = m_objects[i].get();
        }

        m_root = buildBvhRoot<2>(rawObjects, 16);
    }

 private:
    // Search for best hit through BVHs (iterative)
    bool checkBvhHitI(Intersect &_hit) const
    {
        thread_local static Stack<BvhNode<PrimitiveInstance>*> nodes;
        assert(m_root != nullptr);  // check that we built the BVH tree
        nodes.push(m_root.get());
                
        while (nodes.empty() == false) {
            // get last node
            auto pNode = nodes.pop();
            
            // check node hits
            for (const auto &pObj : pNode->m_primitives) {
                Intersect nh(_hit);
                if ( (pObj->hit(nh) == true) &&
                     ( (_hit == false) || (nh.m_fPositionOnRay < _hit.m_fPositionOnRay)) )
                {
                    _hit = nh;
                }
            }
            
            // go down tree
            if ( (pNode->m_right != nullptr) &&
                 (pNode->m_right->intersect(_hit.m_viewRay) == true) ) {
                nodes.push(pNode->m_right.get());
            }

            if ( (pNode->m_left != nullptr) &&
                 (pNode->m_left->intersect(_hit.m_viewRay) == true) ) {
                nodes.push(pNode->m_left.get());
            }
        }
        
        return _hit;
    }

    // Search for best hit through BVHs (recursive)
    bool checkBvhHitR(Intersect &_hit, const std::unique_ptr<BvhNode<PrimitiveInstance>> &_pNode) const
    {
        if (_pNode->empty() == false) {
            for (const auto &pObj : _pNode->m_primitives) {
                Intersect nh(_hit);
                if ( (pObj->hit(nh) == true) &&
                     ( (_hit == false) || (nh.m_fPositionOnRay < _hit.m_fPositionOnRay)) )
                {
                    _hit = nh;
                }
            }
        }

        if ( (_pNode->m_left != nullptr) &&
             (_pNode->m_left->intersect(_hit.m_viewRay) == true) ) {
            checkBvhHitR(_hit, _pNode->m_left);
        }
        
        if ( (_pNode->m_right != nullptr) &&
             (_pNode->m_right->intersect(_hit.m_viewRay) == true) ) {
            checkBvhHitR(_hit, _pNode->m_right);
        }
        
        return _hit;
    }
    
 private:
    std::unique_ptr<BvhNode<PrimitiveInstance>>      m_root;
};


// Factory responsible for creating the correct scene and camera
class Loader
{
 public:
    virtual ~Loader() = default;
    virtual std::unique_ptr<Scene> loadScene() const = 0;
    virtual std::unique_ptr<Camera> loadCamera() const = 0;
};


class MainWindow : public QMainWindow
{
 protected:
    using clock_type = std::chrono::high_resolution_clock;
    
 public:
    MainWindow(const std::unique_ptr<Loader> &&_pLoader)
        :QMainWindow(),
         m_iFrameCount(0),
         m_bFrameDone(false),
         m_iWidth(1024),
         m_iHeight(768),
         m_iNumWorkers(std::max(std::thread::hardware_concurrency() * 2, 2u)),
         m_iMaxSamplesPerPixel(128),
         m_iMaxTraceDepth(64),
         m_fColorTollerance(0.0f),
         m_uRandSeed(1)
    {
        resize(m_iWidth, m_iHeight);
        setWindowTitle(QApplication::translate("windowlayout", "Raytracer"));
        startTimer(200, Qt::PreciseTimer);
        
        m_pViewport = std::make_unique<Viewport>(m_iWidth, m_iHeight);
        m_pCamera = _pLoader->loadCamera();
        m_pScene = _pLoader->loadScene();
    }
    
 protected:
    virtual void paintEvent(QPaintEvent *_event) {
        QPainter painter(this);
        QImage image(m_iWidth, m_iHeight, QImage::Format_RGB888);

        if (m_pSource != nullptr) {
            std::memcpy(image.bits(),
                        m_pSource->image().data(),
                        m_pSource->image().size());
        }
        else {
            image.fill(Qt::black);
        }
        
        painter.drawImage(0, 0, image);
        m_iFrameCount++;
    }
    
    virtual void timerEvent(QTimerEvent *_event) {
        if (m_pSource == nullptr)
        {
            m_tpInit = clock_type::now();
            m_pSource = std::make_unique<Frame>(m_pViewport.get(),
                                                m_pCamera.get(),
                                                m_pScene.get(),
                                                m_iNumWorkers,
                                                m_iMaxSamplesPerPixel,
                                                m_iMaxTraceDepth,
                                                m_fColorTollerance,
                                                m_uRandSeed);
        }
        else {
            m_pSource->updateFrameProgress();
            printf("active jobs=%d, progress=%.2f, time_to_finish=%.2fs, total_time=%.2fs, rays_ps=%.2f\n",
                    (int)m_pSource->activeJobs(), m_pSource->progress(), m_pSource->timeToFinish(), m_pSource->timeTotal(), m_pSource->raysPerSecond());
            
            if (m_pSource->isFinished() == true) {
                if (m_bFrameDone == false) {
                    m_pSource->writeJpegFile("raytraced.jpeg", 100);
                    m_bFrameDone = true;
                    
                    auto td = clock_type::now() - m_tpInit;
                    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(td).count();
                    
                    std::string title = std::string("Done ") + std::to_string((float)ns/1e09) + "s";
                    setWindowTitle(QString::fromStdString(title));
                }
            }
        }
        
        this->update(this->rect());
    }
    
 private:
    std::unique_ptr<Scene>              m_pScene;
    std::unique_ptr<Viewport>           m_pViewport;
    std::unique_ptr<Camera>             m_pCamera;
    std::unique_ptr<LNF::Frame>         m_pSource;
    int                                 m_iFrameCount;
    bool                                m_bFrameDone;
    clock_type::time_point              m_tpInit;
    int                                 m_iWidth;
    int                                 m_iHeight;
    int                                 m_iNumWorkers;
    int                                 m_iMaxSamplesPerPixel;
    int                                 m_iMaxTraceDepth;
    float                               m_fColorTollerance;
    uint32_t                            m_uRandSeed;
};




// scene -- mandlebulb
class LoaderScene0  : public Loader
{
 public:
    virtual std::unique_ptr<Scene> loadScene() const override {
        auto pScene = std::make_unique<SimpleSceneBvh>();
        auto pAO = createMaterial<FakeAmbientOcclusion>(pScene);
        auto pGlow = createMaterial<Glow>(pScene);
        auto pLightWhite = createMaterial<Light>(pScene, Color(30.0, 30.0, 30.0));

        createPrimitiveInstance<Sphere>(pScene, axisTranslation(Vec(0, 200, 100)), 30, pLightWhite);
        createPrimitiveInstance<MarchedMandle>(pScene, axisEulerZYX(0, 0, 0, Vec(0, 0, 0), 40.0), pGlow);
        
        pScene->build();   // build BVH
        return pScene;
    }

    virtual std::unique_ptr<Camera> loadCamera() const override {
        return std::make_unique<SimpleCamera>(Vec(50, 0, 30), Vec(0, 1, 0), Vec(0, 0, 15), deg2rad(60), 5.0, 15);
    }
};


// scene -- raymarching
class LoaderScene1  : public Loader
{
 public:
    virtual std::unique_ptr<Scene> loadScene() const override {
        auto pScene = std::make_unique<SimpleSceneBvh>();
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
        auto pScene = std::make_unique<SimpleSceneBvh>();
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
        auto pScene = std::make_unique<SimpleSceneBvh>();
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


int main(int argc, char *argv[])
{
    auto pLoader = std::make_unique<LoaderScene0>();

    // start app
    QApplication app(argc, argv);
    MainWindow window(std::move(pLoader));
    window.show();

    return app.exec();
}
