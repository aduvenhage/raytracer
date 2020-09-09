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
        double shift = _ray.m_direction.m_dY * _ray.m_direction.m_dY * 0.3 + 0.3;
        return Color(shift, shift, 0.6);
    }

    /*
     Add a new shape to the scene.
     The scene is expected to be static (thread-safe).  Only do this when not rendering.
     */
    virtual void addShape(const std::shared_ptr<Shape> &_pShape) override {
        m_shapes.push_back(_pShape);
    }
    
 protected:
    std::vector<std::shared_ptr<Shape>>   m_shapes;
};



int raytracer()
{
    int width = 1920;
    int height = 1200;
    int fov = 60;
    int numWorkers = 16;
    int samplesPerPixel = 4;
    int maxTraceDepth = 32;

    // init
    HighPrecisionScopeTimer timer;
    OutputImageBuffer image(width, height);
    auto pView = std::make_shared<ViewportScreen>(width, height, fov);
    auto pScene = std::make_shared<SimpleScene>();
    auto pCamera = std::make_shared<Camera>(Vec(0, 20, 80), Vec(0, 1, 0), Vec(0, 0, -10));
    pView->setCamera(pCamera);
    
    // create scene
    auto pDiffuse1 = std::make_shared<DiffuseCheckered>(Color(1.0, 0.8, 0.1), Color(1.0, 0.2, 0.1), 8);
    auto pDiffuse2 = std::make_shared<DiffuseCheckered>(Color(1.0, 1.0, 1.0), Color(0.2, 0.2, 0.2), 8);
    auto pGlass1 = std::make_shared<Glass>(Color(0.9, 0.9, 0.9), 0.01, 1.5);
    auto pMetal1 = std::make_shared<Metal>(Color(0.8, 0.8, 0.8), 0.05);
    auto pLight1 = std::make_shared<Light>(Color(10.0, 10.0, 10.0));

    pScene->addShape(std::make_shared<Plane>(pDiffuse1));
    pScene->addShape(std::make_shared<Transform>(std::make_shared<Sphere>(15, pGlass1), axisIdentity(), Vec(0, 15, 10)));
    pScene->addShape(std::make_shared<Transform>(std::make_shared<Sphere>(15, pDiffuse2), axisIdentity(), Vec(30, 15, -20)));
    pScene->addShape(std::make_shared<Transform>(std::make_shared<Sphere>(15, pDiffuse2), axisEulerZYX(0, 0.5, 0), Vec(-30, 15, -20)));
    pScene->addShape(std::make_shared<Transform>(std::make_shared<Box>(Vec(5, 10, 5), pDiffuse2), axisEulerZYX(0, 1, 0), Vec(10, 5, 30)));
    pScene->addShape(std::make_shared<Transform>(std::make_shared<Box>(Vec(5, 10, 5), pMetal1), axisEulerZYX(0, 0.2, 0), Vec(-10, 5, 30)));
    pScene->addShape(std::make_shared<Transform>(std::make_shared<Box>(Vec(5, 10, 5), pGlass1), axisEulerZYX(0, 0.5, 0), Vec(0, 5, 40)));
    pScene->addShape(std::make_shared<Transform>(std::make_shared<Sphere>(15, pLight1), axisIdentity(), Vec(0, 60, 20)));

    
    // render frame
    LNF::Frame frame(pView, pScene, numWorkers, samplesPerPixel, maxTraceDepth);
    for (;;) {
        int active = frame.activeJobs();
        if (active == 0) {
            break;
        }
        else {
            printf("%d\n", active);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    frame.writeJpegFile("raytraced.jpeg", 100);
    
    return 0;
}


// just create mandlebrot image and output to JPEG
int mandlebrot()
{
    int width = 1280;
    int height = 960;
    auto mb = MandleBrot(width, height);
    mb.render();
    mb.writeToJpeg("mandlebrot.jpeg");
    return 0;
}



class MainWindow : public QMainWindow
{
 public:
    MainWindow()
        :QMainWindow(),
         m_iFrameCount(0)
    {
        resize(1024, 700);
        setWindowTitle(QApplication::translate("windowlayout", "Window layout"));
        
    }
    
 protected:
    virtual void paintEvent(QPaintEvent *_event) {
        QPainter painter(this);
        QImage image(width(), height(), QImage::Format_RGB888);

        uchar *pData = image.bits();
        
        for (int y = 0; y < height(); y++) {
            for (int x = 0; x < width(); x++) {
                *pData++ = (int)(127.0 * sin(x/20.0 + m_iFrameCount/10.0) + 127.0 + 0.5);
                *pData++ = (int)(127.0 * cos(y/20.0 + m_iFrameCount/10.0) + 127.0 + 0.5);
                *pData++ = 0;
            }
        }
        
        painter.drawImage(0, 0, image);
        m_iFrameCount++;
    }
    
 private:
    int     m_iFrameCount;
};



int main(int argc, char *argv[])
{
    raytracer();
    
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    
    return app.exec();
}
