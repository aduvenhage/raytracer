#include "lnf/box.h"
#include "lnf/bvh.h"
#include "lnf/color.h"
#include "lnf/constants.h"
#include "lnf/default_materials.h"
#include "lnf/frame.h"
#include "lnf/jobs.h"
#include "lnf/jpeg.h"
#include "lnf/mandlebrot.h"
#include "lnf/marched_materials.h"
#include "lnf/marched_node.h"
#include "lnf/mesh.h"
#include "lnf/outputimage.h"
#include "lnf/plane.h"
#include "lnf/profile.h"
#include "lnf/ray.h"
#include "lnf/scene.h"
#include "lnf/transform.h"
#include "lnf/smoke_box.h"
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
    
 public:
    SimpleScene()
        :m_dTimeOnBvhS(0),
         m_dTimeOnHitsS(0)
    {}
    
    /*
       Checks for an intersect with a scene object.
       Could be accessed by multiple worker threads concurrently.
     */
    virtual bool hit(Intersect &_hit, const Ray &_ray, RandomGen &_randomGen) const override {
        bool bHit = false;
        
        // find possible hit nodes from BVH
        auto tpInit = clock_type::now();

        static thread_local std::vector<Node*> nodes;
        nodes.clear();
        m_bvhRoot.intersect(nodes, _ray);
        auto tpBvh = clock_type::now();

        // find best hit
        for (auto &pNode : nodes) {
            Intersect nh;
            if ( (pNode->hit(nh, _ray, _randomGen) == true) &&
                 ((bHit == false) || (nh.m_fPositionOnRay < _hit.m_fPositionOnRay)) )
            {
                _hit = nh;
                bHit = true;
            }
        }
        
        auto tpHit = clock_type::now();
        
        // update hit timing
        m_dTimeOnBvhS += std::chrono::duration_cast<std::chrono::nanoseconds>(tpBvh - tpInit).count() * 1e-09;
        m_dTimeOnHitsS += std::chrono::duration_cast<std::chrono::nanoseconds>(tpHit - tpBvh).count() * 1e-09;
        
        return bHit;
    }
    
    /*
     Checks for the background color (miss handler).
     Could be accessed by multiple worker threads concurrently.
     */
    virtual Color backgroundColor() const override {
        return Color(0.5f, 0.5f, 0.5f);
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
        std::vector<Node*> vecPtrs;
        vecPtrs.reserve(m_nodes.size());
        for (auto &p : m_nodes) {
            vecPtrs.push_back(p.get());
        }
        
        m_bvhRoot.build(vecPtrs);
    }
    
    double getTimeOnBvhS() const {
        return m_dTimeOnBvhS;
    }
    
    double getTimeOnHitsS() const {
        return m_dTimeOnHitsS;
    }
    
 protected:
    std::vector<std::unique_ptr<Node>>   m_nodes;
    BvhNode<Node>                        m_bvhRoot;
    mutable double                       m_dTimeOnBvhS;
    mutable double                       m_dTimeOnHitsS;
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
         m_iNumWorkers(std::max(std::thread::hardware_concurrency() * 2, 4u)),
         m_iMaxSamplesPerPixel(64),
         m_iMaxTraceDepth(16),
         m_fColorTollerance(0.00001)
    {
        resize(m_iWidth, m_iHeight);
        setWindowTitle(QApplication::translate("windowlayout", "Raytracer"));
        startTimer(std::chrono::milliseconds(100));
        
        m_pView = std::make_unique<ViewportScreen>(m_iWidth, m_iHeight, m_fFov);
        m_pCamera = std::make_unique<SimpleCamera>(Vec(0, 20, 200), Vec(0, 1, 0), Vec(0, 5, 0), 1.0, 150);
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
                    
                    printf("time on bvh = %.2f, time on hits = %.2f\n",
                            (float)m_pScene->getTimeOnBvhS()/m_iNumWorkers,
                            (float)m_pScene->getTimeOnHitsS()/m_iNumWorkers);
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



int main(int argc, char *argv[])
{
    // init
    auto pScene = std::make_unique<SimpleScene>();
    RandomGen generator{std::random_device()()};
    
    // create scene
    auto pDiffuse0 = std::make_unique<Diffuse>(Color(0.2, 0.2, 0.2));
    auto pDiffuse1 = std::make_unique<DiffuseCheckered>(Color(1.0, 1.0, 1.0), Color(1.0, 0.4, 0.2), 2);
    auto pDiffuse2 = std::make_unique<DiffuseCheckered>(Color(1.0, 1.0, 1.0), Color(0.2, 0.2, 0.2), 2);
    auto pDiffuse3 = std::make_unique<Diffuse>(Color(0.9, 0.1, 0.1));
    auto pDiffuse4 = std::make_unique<Diffuse>(Color(0.1, 0.9, 0.1));
    auto pDiffuse5 = std::make_unique<Diffuse>(Color(0.1, 0.1, 0.9));
    auto pDiffuse6 = std::make_unique<Diffuse>(Color(0.9, 0.9, 0.9));
    auto pGlass1 = std::make_unique<Glass>(Color(0.8, 0.8, 0.8), 0.01, 1.8);
    auto pGlass2 = std::make_unique<Glass>(Color(0.5, 0.5, 0.5), 0.01, 1.8);
    auto pMetal1 = std::make_unique<Metal>(Color(0.8, 0.8, 0.8), 0.04);
    auto pLight1 = std::make_unique<Light>(Color(20.0, 20.0, 20.0));
    auto pLight2 = std::make_unique<Light>(Color(1.0, 1.0, 1.0));
    auto pLight3 = std::make_unique<Light>(Color(1.0, 0.1, 0.1));
    auto pLight4 = std::make_unique<Light>(Color(0.1, 30.0, 0.1));
    auto pLight5 = std::make_unique<Light>(Color(0.1, 0.1, 1.0));
    auto pNormalsInside = std::make_unique<SurfaceNormal>(false);
    auto pTraingleRgb1 = std::make_unique<TriangleRGB>();
    auto pMarched1 = std::make_unique<GlassSwirl>(0.01, 0.1, 1.8);
    auto pMarched2 = std::make_unique<GlassBubbles>(0.01, 0.1, 1.8);
    auto pMarched3 = std::make_unique<Swirl>(0.01, 1.8);
    auto pMarched4 = std::make_unique<MarchedSphere>(10, 0.01, 1.8);
    auto pMarched5 = std::make_unique<MarchedCloud>(0.01, 1.8);

    pScene->addNode(std::make_unique<Disc>(500, pDiffuse1.get()));
    pScene->addNode(std::make_unique<Transform>(std::make_unique<Sphere>(50, pLight1.get()), axisTranslation(Vec(0, 200, 0))));
    
    //pScene->addNode(std::make_unique<SmokeBox>(300, pGlass1.get(), 250));

    pScene->addNode(std::make_unique<Transform>(std::make_unique<MarchedSwirl>(40, pMetal1.get(), 100), axisEulerZYX(0, 0, 0, Vec(0, 20, 60))));
    pScene->addNode(std::make_unique<Transform>(std::make_unique<Sphere>(20, pDiffuse4.get()), axisEulerZYX(0, 0, 0, Vec(-40, 20, 10))));
    pScene->addNode(std::make_unique<Transform>(std::make_unique<Sphere>(20, pDiffuse5.get()), axisEulerZYX(0, 3, 0, Vec(40, 20, 10))));

    pScene->addNode(std::make_unique<Transform>(std::make_unique<Sphere>(8, pLight4.get()), axisEulerZYX(0, 0, 0, Vec(200, 8, -150))));
    
    //pScene->addNode(std::make_unique<Transform>(std::make_unique<Sphere>(15, pMarched5.get()), axisEulerZYX(0, 0, 0, Vec(0, 15, 0))));
    //pScene->addNode(std::make_unique<Transform>(std::make_unique<Sphere>(4, pDiffuse2.get()), axisEulerZYX(0, 0, 1, Vec(-20, 8, 20))));
    //pScene->addNode(std::make_unique<Transform>(std::make_unique<Sphere>(4, pDiffuse2.get()), axisEulerZYX(0, 1, 0, Vec(20, 8, 20))));
    
    //pScene->addNode(std::make_unique<Transform>(std::make_unique<Sphere>(8, pMetal1.get()), axisEulerZYX(0, 1, 0, Vec(-8, 8, 0))));
    //pScene->addNode(std::make_unique<Transform>(std::make_unique<Sphere>(8, pMetal1.get()), axisEulerZYX(0, 0, 0, Vec(8, 8, 0))));
    
    
    pScene->build();

    // start app
    QApplication app(argc, argv);
    MainWindow window(pScene.get());
    window.show();
    
    return app.exec();
}
