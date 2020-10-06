#include "lnf/box.h"
#include "lnf/bvh.h"
#include "lnf/color.h"
#include "lnf/constants.h"
#include "lnf/default_materials.h"
#include "lnf/frame.h"
#include "lnf/jobs.h"
#include "lnf/jpeg.h"
#include "lnf/mandlebrot.h"
#include "lnf/outputimage.h"
#include "lnf/plane.h"
#include "lnf/profile.h"
#include "lnf/ray.h"
#include "lnf/scene.h"
#include "lnf/transform.h"
#include "lnf/sphere.h"
#include "lnf/trace.h"
#include "lnf/triangle.h"
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
        return m_baseColor * (m_mandlebrot.value(_hit.m_uv.m_fU, _hit.m_uv.m_fV) * 0.1 + 0.1);
    }
    
 private:
    MandleBrot      m_mandlebrot;
    Color           m_baseColor;
};


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
    virtual bool hit(Intersect &_hit, const Ray &_ray) const override {
        static thread_local std::vector<Node*> nodes;
        bool bHit = false;
        
        nodes.clear();
        m_bvhRoot.intersect(nodes, _ray);
        
        // find best hit
        Intersect nh;
        for (auto &pNode : nodes) {
            if ( (pNode->hit(nh, _ray) == true) &&
                 ((bHit == false) || (nh.m_fPositionOnRay < _hit.m_fPositionOnRay)) )
            {
                _hit = nh;
                bHit = true;
            }
        }
        
        return bHit;
    }
    
    /*
     Checks for the background color (miss handler).
     Could be accessed by multiple worker threads concurrently.
     */
    virtual Color missColor(const Ray &_ray) const override {
        float shift = _ray.m_direction.m_fY * _ray.m_direction.m_fY * 0.3f + 0.3f;
        return Color(shift, shift, 0.6f);
    }

    /*
     Add a new node to the scene.
     The scene is expected to be static (thread-safe).  Only do this when not rendering.
     */
    virtual void addNode(std::unique_ptr<Node> &&_pNode) override {
        m_nodes.push_back(std::move(_pNode));
    }
    
    /*
     Build scene graph.
     */
    void build() {
        m_bvhRoot.build(m_nodes);
    }
    
 protected:
    std::vector<std::shared_ptr<Node>>   m_nodes;
    BvhNode                              m_bvhRoot;
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
         m_bFrameDone(false)
    {
        int width = 1024;
        int height = 768;
        int fov = 60;

        resize(width, height);
        setWindowTitle(QApplication::translate("windowlayout", "Raytracer"));
        startTimer(std::chrono::milliseconds(100));
        
        m_pView = std::make_unique<ViewportScreen>(width, height, fov);
        m_pCamera = std::make_unique<SimpleCamera>(Vec(0, 20, 100), Vec(0, 1, 0), Vec(0, 0, -10), 0.5, 100);
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
            int numWorkers = std::max(std::thread::hardware_concurrency() * 2, 4u);
            int samplesPerPixel = 32;
            int maxTraceDepth = 32;
            
            m_tpInit = clock_type::now();
            m_pSource = std::make_unique<Frame>(m_pView.get(), m_pScene, numWorkers, samplesPerPixel, maxTraceDepth);
        }
        else if (m_pSource->isFinished() == true) {
            if (m_bFrameDone == false) {
                m_pSource->writeJpegFile("raytraced.jpeg", 100);
                m_bFrameDone = true;
                
                auto td = clock_type::now() - m_tpInit;
                auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(td).count();
                
                std::string title = std::string("Done ") + std::to_string((float)ns/1e09) + "s";
                setWindowTitle(QString::fromStdString(title));
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
};



int main(int argc, char *argv[])
{
    // init
    auto pScene = std::make_unique<SimpleScene>();
    RandomGen generator;
    
    // create scene
    auto pDiffuse0 = std::make_unique<Diffuse>(Color(0.4, 0.4, 0.4));
    auto pDiffuse1 = std::make_unique<DiffuseCheckered>(Color(1.0, 0.8, 0.1), Color(1.0, 0.2, 0.1), 8);
    auto pDiffuse2 = std::make_unique<DiffuseCheckered>(Color(1.0, 1.0, 1.0), Color(0.2, 0.2, 0.2), 8);
    auto pDiffuse3 = std::make_unique<Diffuse>(Color(0.8, 0.4, 0.4));
    auto pGlass1 = std::make_unique<Glass>(Color(0.8, 0.8, 0.8), 0.01, 1.8);
    auto pGlass2 = std::make_unique<Glass>(Color(0.5, 0.5, 0.5), 0.03, 1.8);
    auto pMetal1 = std::make_unique<Metal>(Color(0.8, 0.8, 0.8), 0.04);
    auto pLight1 = std::make_unique<Light>(Color(10.0, 10.0, 10.0));
    auto pLight2 = std::make_unique<Light>(Color(1.0, 1.0, 1.0));
    auto pLight3 = std::make_unique<Light>(Color(1.0, 0.1, 0.1));
    auto pLight4 = std::make_unique<Light>(Color(0.1, 1.0, 0.1));
    auto pLight5 = std::make_unique<Light>(Color(0.1, 0.1, 1.0));
    auto pNormalsInside = std::make_unique<SurfaceNormal>(false);
    auto pTraingleRgb1 = std::make_unique<TriangleRGB>();
    
    auto materials = std::vector<Material*>{pDiffuse0.get(), pDiffuse1.get(), pDiffuse2.get(),
                                            pGlass1.get(),
                                            pMetal1.get(),
                                            pLight2.get(), pLight3.get(), pLight4.get(), pLight5.get(),
                                            pNormalsInside.get()};

    pScene->addNode(std::make_unique<Transform>(std::make_unique<Disc>(500, pDiffuse0.get()), axisEulerZYX(0, 0, 0)));
    pScene->addNode(std::make_unique<Transform>(std::make_unique<Sphere>(15, pGlass1.get()), axisEulerZYX(0, 0, 0, Vec(0, 15, 0))));
    
    pScene->addNode(std::make_unique<Triangle>(Vec(10, 5, 20),
                                               Vec(25, 25, 25),
                                               Vec(5, 25, 25),
                                               pTraingleRgb1.get()));
    
    
    
    pScene->addNode(std::make_unique<Transform>(std::make_unique<Sphere>(15, pDiffuse2.get()), axisTranslation(Vec(30, 15, -20))));
    pScene->addNode(std::make_unique<Transform>(std::make_unique<Sphere>(15, pDiffuse2.get()), axisEulerZYX(0, 0.5, 0, Vec(-30, 15, -20))));
    pScene->addNode(std::make_unique<Transform>(std::make_unique<Box>(Vec(5, 10, 5), pDiffuse2.get()), axisEulerZYX(0, 1, 0, Vec(10, 5.5, 30))));
    pScene->addNode(std::make_unique<Transform>(std::make_unique<Box>(Vec(5, 10, 5), pMetal1.get()), axisEulerZYX(0, 0.2, 0, Vec(-10, 5.5, 30))));
    pScene->addNode(std::make_unique<Transform>(std::make_unique<Box>(Vec(5, 5, 5), pGlass2.get()), axisEulerZYX(0, 0, 0, Vec(10, 20, 40))));
    pScene->addNode(std::make_unique<Transform>(std::make_unique<Box>(Vec(5, 5, 5), pGlass2.get()), axisEulerZYX(0, 0.3, 0, Vec(10, 3, 40))));
    pScene->addNode(std::make_unique<Transform>(std::make_unique<Box>(Vec(15, 9, 5), pGlass1.get()), axisEulerZYX(0, -0.2, 0, Vec(-10, 5, 40))));
    pScene->addNode(std::make_unique<Transform>(std::make_unique<Sphere>(15, pLight1.get()), axisTranslation(Vec(0, 100, 40))));
    
    /*
    std::uniform_real_distribution<float> dp(-500, 500);
    std::uniform_real_distribution<float> dr(0, LNF::pi);
    
    for (int i = 0; i < 5000; i++) {
        pScene->addNode(std::make_unique<Transform>(std::make_unique<Sphere>(5,
                                                                             materials[i % materials.size()]),
                                                                             axisEulerZYX(0, dr(generator), 0, Vec(dp(generator), 5, dp(generator)))));
    }
    */
    
    pScene->build();

    // start app
    QApplication app(argc, argv);
    MainWindow window(pScene.get());
    window.show();
    
    return app.exec();
}
