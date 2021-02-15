#include "lnf/box.h"
#include "lnf/bvh.h"
#include "lnf/color.h"
#include "lnf/constants.h"
#include "lnf/default_materials.h"
#include "lnf/frame.h"
#include "lnf/jobs.h"
#include "lnf/jpeg.h"
#include "lnf/mandlebrot.h"
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

#include <vector>
#include <memory>
#include <random>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <iostream>

#include <QtWidgets>


using namespace LNF;


// diffuse material
class DiffuseMandlebrot : public Diffuse
{
 public:
    DiffuseMandlebrot()
        :Diffuse(Color()),
         m_mandlebrot(1, 1),
         m_baseColor(0.4, 0.2, 0.1)
    {}
    
    /* Returns the diffuse color at the given surface position */
    virtual Color color(const Intersect &_hit) const override {
        return m_baseColor * (m_mandlebrot.value(_hit.m_uv.u(), _hit.m_uv.v()) * 0.1 + 0.1);
    }
    
 private:
    MandleBrot      m_mandlebrot;
    Color           m_baseColor;
};


// simple scene with a linear search for object hits
class SimpleScene   : public Scene
{
 protected:
    using clock_type = std::chrono::high_resolution_clock;
    using PrimitiveTree = BvhTree<PrimitiveInstance, 24, 1>;
    
 public:
    SimpleScene()
    {}
    
    /*
       Checks for an intersect with a scene object.
       Could be accessed by multiple worker threads concurrently.
     */
    virtual bool hit(Intersect &_hit, RandomGen &_randomGen) const override {
        bool bHit = false;
        Intersect bh(_hit);
        
        // find potential hits
        thread_local static std::vector<const PrimitiveInstance*> primeObjects;
        primeObjects.clear();
        findHittables(primeObjects, _hit.m_ray);
        
        // find best hit from potentials
        for (auto &pObj : primeObjects) {
            Intersect nh(_hit);
            
            if ( (pObj->hit(nh, _randomGen) == true) &&
                 ((bHit == false) || (nh.m_fPositionOnRay < bh.m_fPositionOnRay)) )
            {
                bh = nh;
                bHit = true;
            }
        }
        
        if (bHit == true) {
            _hit = bh;
        }
        
        return bHit;
    }
    
    /*
        Find primitives potentially hit by ray.
     */
     void findHittables(std::vector<const PrimitiveInstance*> &_primitives, const Ray _ray) const {
        BvhTree<PrimitiveInstance>::intersect(_primitives, m_root, _ray);
        
        /*
        for (const auto &primitive : m_objects) {
            _primitives.push_back(primitive.get());
        }
        */
    }
    
    /*
     Checks for the background color (miss handler).
     Could be accessed by multiple worker threads concurrently.
     */
    virtual Color backgroundColor() const override {
        return Color(0.5f, 0.5f, 0.5f);
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

    /*
        Build acceleration structures
     */
     void build() {
        std::vector<const PrimitiveInstance*> rawObjects;
        rawObjects.reserve(m_objects.size());
        for (auto &pObj : m_objects) {
            rawObjects.push_back(pObj.get());
        }
        
        m_root = PrimitiveTree::build(rawObjects);
     }

 protected:
    std::vector<std::unique_ptr<Resource>>           m_resources;
    std::vector<std::unique_ptr<PrimitiveInstance>>  m_objects;
    PrimitiveTree::node_ptr_type                     m_root;
};


class MainWindow : public QMainWindow
{
 protected:
    using clock_type = std::chrono::high_resolution_clock;
    
 public:
    MainWindow(const SimpleScene *_pScene)
        :QMainWindow(),
         m_pScene(_pScene),
         m_iFrameCount(0),
         m_bFrameDone(false),
         m_iWidth(1024),
         m_iHeight(768),
         m_fFov(60),
         m_iNumWorkers(std::max(std::thread::hardware_concurrency() * 2, 2u)),
         m_iMaxSamplesPerPixel(1024),
         m_iMaxTraceDepth(32),
         m_fColorTollerance(0.00000000001)
    {
        resize(m_iWidth, m_iHeight);
        setWindowTitle(QApplication::translate("windowlayout", "Raytracer"));
        startTimer(std::chrono::milliseconds(100));
        
        m_pView = std::make_unique<ViewportScreen>(m_iWidth, m_iHeight, m_fFov);
        m_pCamera = std::make_unique<SimpleCamera>(Vec(0, 60, 200), Vec(0, 1, 0), Vec(0, 5, 0), 1.5, 200);
        m_pView->setCamera(m_pCamera.get());
    }
    
 protected:
    virtual void paintEvent(QPaintEvent *_event) {
        QPainter painter(this);
        QImage image(m_pView->width(), m_pView->height(), QImage::Format_RGB888);

        if (m_pSource != nullptr) {
            uchar *pDstData = image.bits();
            uchar *pSrcData = m_pSource->image().data();
            size_t n = m_pSource->image().size();
            std::memcpy(pDstData, pSrcData, n);
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
            m_pSource = std::make_unique<Frame>(m_pView.get(),
                                                m_pScene,
                                                m_iNumWorkers,
                                                m_iMaxSamplesPerPixel,
                                                m_iMaxTraceDepth,
                                                m_fColorTollerance);
        }
        else {
            m_pSource->updateFrameProgress();
            printf("active jobs = %d, progress = %.2f, time to finish = %.2fs, total time = %.2fs\n",
                    m_pSource->activeJobs(), m_pSource->progress(), m_pSource->timeToFinish(), m_pSource->timeTotal());
            
            if (m_pSource->isFinished() == true) {
                if (m_bFrameDone == false) {
                    m_pSource->writeJpegFile("raytraced.jpeg", 100);
                    m_bFrameDone = true;
                    
                    auto td = clock_type::now() - m_tpInit;
                    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(td).count();
                    
                    std::string title = std::string("Done ") + std::to_string((float)ns/1e09) + "s";
                    setWindowTitle(QString::fromStdString(title));


                    /*
                    printf("time on bvh = %.2f, time on hits = %.2f\n",
                            (float)m_pScene->getTimeOnBvhS()/m_iNumWorkers,
                            (float)m_pScene->getTimeOnHitsS()/m_iNumWorkers);
                    */



                }
            }
        }
        
        update();
    }
    
 private:
    const SimpleScene                   *m_pScene;
    std::unique_ptr<ViewportScreen>     m_pView;
    std::unique_ptr<Camera>             m_pCamera;
    std::unique_ptr<LNF::Frame>         m_pSource;
    int                                 m_iFrameCount;
    bool                                m_bFrameDone;
    clock_type::time_point              m_tpInit;
    int                                 m_iWidth;
    int                                 m_iHeight;
    float                               m_fFov;
    int                                 m_iNumWorkers;
    int                                 m_iMaxSamplesPerPixel;
    int                                 m_iMaxTraceDepth;
    float                               m_fColorTollerance;
};




void profileBvh() {
    
    RandomGen generator{};
    
    // create scene
    auto dist = std::uniform_real_distribution<float>(-100, 100);
    auto pScene = std::make_unique<SimpleScene>();
    int n = 500;
    for (int i = 0; i < n; i++) {
        
        float x = dist(generator);
        float y = dist(generator);
        float z = dist(generator);

        createPrimitiveInstance<SphereMesh>(pScene.get(), axisEulerZYX(0, 0, 0, Vec(x, y, z)), 16, 8, 4, nullptr);
    }
    
    pScene->build();
    
    // test hits
    ScopeTimer<std::chrono::high_resolution_clock> timer;
    
    for (int i = 0; ; i++) {
        auto ray = randomUnitSphere(generator);
        
        thread_local static std::vector<const PrimitiveInstance*> primitives;
        primitives.clear();
        pScene->findHittables(primitives, Ray(Vec(), ray));
    }
}


int main(int argc, char *argv[])
{
    //profileBvh();
    
    // init
    auto pScene = std::make_unique<SimpleScene>();
    RandomGen generator{std::random_device()()};
    
    // create scene
    auto pDiffuseFloor = createMaterial<DiffuseCheckered>(pScene.get(), Color(1.0, 1.0, 1.0), Color(1.0, 0.4, 0.2), 2);
    auto pGlass = createMaterial<Glass>(pScene.get(), Color(0.95, 0.95, 0.95), 0.01, 1.8);
    auto pMirror = createMaterial<Metal>(pScene.get(), Color(0.95, 0.95, 0.95), 0.02);
    auto pGlow = createMaterial<DiffuseIterations>(pScene.get());
    auto pLightWhite = createMaterial<Light>(pScene.get(), Color(40.0, 40.0, 40.0));
    auto pLightGreen = createMaterial<Light>(pScene.get(), Color(5, 30.0, 5));
    
    createPrimitiveInstance<Disc>(pScene.get(), axisIdentity(), 500, pDiffuseFloor);
    createPrimitiveInstance<Rectangle>(pScene.get(), axisTranslation(Vec(0, 1, 0)), 100, 80, pMirror);
    //createPrimitiveInstance<SmokeBox>(pScene.get(), axisIdentity(), 400, pGlass, 400);
    
    createPrimitiveInstance<Sphere>(pScene.get(), axisTranslation(Vec(0, 200, 0)), 30, pLightWhite);
    createPrimitiveInstance<Sphere>(pScene.get(), axisTranslation(Vec(200, 8, -150)), 8, pLightGreen);

    createPrimitiveInstance<MarchedMandle>(pScene.get(), axisEulerZYX(0, 1, 0, Vec(-50, 45, 0), 40.0), pGlow);
    createPrimitiveInstance<MarchedSphere>(pScene.get(), axisEulerZYX(0, 1, 0, Vec(50, 45, 0), 40.0), 2.0f, pGlass, 0.04f);

    /*
    auto pDiffuseRed = createMaterial<DiffuseCheckered>(pScene.get(), Color(1.0, 1.0, 1.0), Color(0.2, 0.2, 0.2), 20);
    auto pDiffuseGreen = createMaterial<Diffuse>(pScene.get(), Color(0.1, 0.9, 0.1));
    auto pDiffuseBlue = createMaterial<Diffuse>(pScene.get(), Color(0.1, 0.9, 0.1));
    auto pDiffuseFloor = createMaterial<DiffuseCheckered>(pScene.get(), Color(1.0, 1.0, 1.0), Color(1.0, 0.4, 0.2), 2);
    auto pMesh1 = createPrimitive<SphereMesh>(pScene.get(), 32, 16, 4, pDiffuseBlue);

    int n = 50;
    for (int i = 0; i < n; i++) {
        
        float x = 100 * sin((float)i / n * LNF::pi * 2);
        float y = 20 * (cos((float)i / n * LNF::pi * 8) + 1);
        float z = 100 * cos((float)i / n * LNF::pi * 2);

        //createPrimitiveInstance<Sphere>(pScene.get(), axisEulerZYX(0, 0, 0, Vec(x, y, z)), 4, pDiffuseRed);
        //createPrimitiveInstance<SphereMesh>(pScene.get(), axisEulerZYX(0, 0, 0, Vec(x, y, z)), 32, 16, 4, pDiffuseGreen);
        createPrimitiveInstance(pScene.get(), axisEulerZYX(0, 0, 0, Vec(x, y, z)), pMesh1);
    }
    */
    
    pScene->build();

    // start app
    QApplication app(argc, argv);
    MainWindow window(pScene.get());
    window.show();

    return app.exec();
}
