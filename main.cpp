#include "headers/box.h"
#include "headers/color.h"
#include "headers/constants.h"
#include "headers/default_materials.h"
#include "headers/frame.h"
#include "headers/jobs.h"
#include "headers/jpeg.h"
#include "headers/mandlebrot.h"
#include "headers/outputimage.h"
#include "headers/plane.h"
#include "headers/profile.h"
#include "headers/ray.h"
#include "headers/scene.h"
#include "headers/shape_transform.h"
#include "headers/sphere.h"
#include "headers/trace.h"
#include "headers/uv.h"
#include "headers/vec3.h"
#include "headers/viewport.h"

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
        return m_baseColor * (m_mandlebrot.value(_hit.m_uv.m_dU, _hit.m_uv.m_dV) * 0.1 + 0.1);
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
    virtual Intersect hit(const Ray &_ray) const override {
        Intersect ret;

        // find best hit
        for (auto &pShape : m_shapes) {
            auto hit = pShape->intersect(_ray);
            if (hit < ret) {
                ret = hit;
            }
        }
        
        return ret;
    }
    
    /*
     Checks for the background color (miss handler).
     Could be accessed by multiple worker threads concurrently.
     */
    virtual Color missColor(const Ray &_ray) const override {
        float shift = _ray.m_direction.m_dY * _ray.m_direction.m_dY * 0.3f + 0.3f;
        return Color(shift, shift, 0.6f);
    }

    /*
     Add a new shape to the scene.
     The scene is expected to be static (thread-safe).  Only do this when not rendering.
     */
    virtual void addShape(std::unique_ptr<Shape> &&_pShape) override {
        m_shapes.push_back(std::move(_pShape));
    }
    
 protected:
 
    // TODO: FIGURE OUT contigeous storage  .. ECS ??
    std::vector<std::unique_ptr<Shape>>   m_shapes;
};



class MainWindow : public QMainWindow
{
 public:
    MainWindow(const SimpleScene *_pScene)
        :QMainWindow(),
         m_pScene(_pScene),
         m_iFrameCount(0)
    {
        int width = 640;
        int height = 480;
        int fov = 60;

        resize(width, height);
        setWindowTitle(QApplication::translate("windowlayout", "Raytracer"));
        startTimer(std::chrono::milliseconds(500));
        
        m_pView = std::make_unique<ViewportScreen>(width, height, fov);
        m_pCamera = std::make_unique<Camera>(Vec(0, 20, 80), Vec(0, 1, 0), Vec(0, 0, -10));
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
        if ( (m_pSource == nullptr) /*||
             (m_pSource->isFinished() == true)*/ )
        {
            int numWorkers = std::max(std::thread::hardware_concurrency() * 2, 4u);
            int samplesPerPixel = 4096;
            int maxTraceDepth = 32;
            m_pSource = std::make_unique<Frame>(m_pView.get(), m_pScene, numWorkers, samplesPerPixel, maxTraceDepth);
        }

        update();
    }
    
 private:
    const SimpleScene                   *m_pScene;
    std::unique_ptr<ViewportScreen>     m_pView;
    std::unique_ptr<Camera>             m_pCamera;
    std::unique_ptr<LNF::Frame>         m_pSource;
    int                                 m_iFrameCount;
};



int main(int argc, char *argv[])
{
    // init
    auto pScene = std::make_unique<SimpleScene>();
    
    // create scene
    auto pDiffuse1 = std::make_unique<DiffuseCheckered>(Color(1.0, 0.8, 0.1), Color(1.0, 0.2, 0.1), 8);
    auto pDiffuse2 = std::make_unique<DiffuseCheckered>(Color(1.0, 1.0, 1.0), Color(0.2, 0.2, 0.2), 8);
    auto pGlass1 = std::make_unique<Glass>(Color(0.9, 0.9, 0.9), 0.01, 1.5);
    auto pMetal1 = std::make_unique<Metal>(Color(0.8, 0.8, 0.8), 0.05);
    auto pLight1 = std::make_unique<Light>(Color(10.0, 10.0, 10.0));

    pScene->addShape(std::make_unique<Disc>(500, pDiffuse1.get()));
    pScene->addShape(std::make_unique<Transform>(std::make_unique<Sphere>(15, pGlass1.get()), axisIdentity(), Vec(0, 15, 10)));
    pScene->addShape(std::make_unique<Transform>(std::make_unique<Sphere>(15, pDiffuse2.get()), axisIdentity(), Vec(30, 15, -20)));
    pScene->addShape(std::make_unique<Transform>(std::make_unique<Sphere>(15, pDiffuse2.get()), axisEulerZYX(0, 0.5, 0), Vec(-30, 15, -20)));
    pScene->addShape(std::make_unique<Transform>(std::make_unique<Box>(Vec(5, 10, 5), pDiffuse2.get()), axisEulerZYX(0, 1, 0), Vec(10, 5, 30)));
    pScene->addShape(std::make_unique<Transform>(std::make_unique<Box>(Vec(5, 10, 5), pMetal1.get()), axisEulerZYX(0, 0.2, 0), Vec(-10, 5, 30)));
    pScene->addShape(std::make_unique<Transform>(std::make_unique<Box>(Vec(5, 10, 5), pGlass1.get()), axisEulerZYX(0, 0.5, 0), Vec(0, 5, 40)));
    pScene->addShape(std::make_unique<Transform>(std::make_unique<Sphere>(15, pLight1.get()), axisIdentity(), Vec(0, 60, 20)));

    // start app
    QApplication app(argc, argv);
    MainWindow window(pScene.get());
    window.show();
    
    return app.exec();
}
