#include "headers/box.h"
#include "headers/color.h"
#include "headers/constants.h"
#include "headers/jobs.h"
#include "headers/jpeg.h"
#include "headers/mandlebrot.h"
#include "headers/outputimage.h"
#include "headers/plane.h"
#include "headers/profile.h"
#include "headers/ray.h"
#include "headers/scene.h"
#include "headers/sphere.h"
#include "headers/trace.h"
#include "headers/uv.h"
#include "headers/vec3.h"
#include "headers/viewport.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <memory>
#include <random>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <iostream>

using namespace LNF;


// diffuse material
class Diffuse : public Material
{
 public:
    Diffuse(const Color &_color)
        :m_color(_color)
    {}
    
    /* Returns the scattered ray at the intersection point. */
    virtual ScatteredRay scatter(const Intersect &_hit, const Ray &_ray, RandomGen &_randomGen) const override {
        auto scatteredDirection = (_hit.m_normal + randomUnitSphere(_randomGen)).normalized();
        return ScatteredRay(Ray(_hit.m_position, scatteredDirection), color(_hit), Color());
    }
    
 protected:
    /* Returns the diffuse color at the given surface position */
    virtual Color color(const Intersect &_hit) const {return m_color;}
    
 private:
    Color           m_color;
};


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


// diffuse material
class DiffuseCheckered : public Diffuse
{
 public:
    DiffuseCheckered(const Color &_colorA, const Color &_colorB, int _iBlockSize)
        :Diffuse(Color()),
         m_colorA(_colorA),
         m_colorB(_colorB),
         m_iBlockSize(_iBlockSize)
    {}
    
    /* Returns the diffuse color at the given surface position */
    virtual Color color(const Intersect &_hit) const override {
        int c = ((int)(_hit.m_uv.m_dU * m_iBlockSize) + (int)(_hit.m_uv.m_dV * m_iBlockSize)) % 2;
        return m_colorA * c + m_colorB * (1 - c);
    }
    
 private:
    Color           m_colorA;
    Color           m_colorB;
    int             m_iBlockSize;
};


// light emitting material
class Light : public Material
{
 public:
   Light(const Color &_color)
       :m_color(_color)
   {}
   
   /* Returns the scattered ray at the intersection point. */
   virtual ScatteredRay scatter(const Intersect &_hit, const Ray &_ray, RandomGen &_randomGen) const override {
       return ScatteredRay(_ray, Color(), m_color);
   }
   
 private:
   Color            m_color;
};


// shiny metal material
class Metal : public Material
{
 public:
    Metal(const Color &_color, double _dScatter)
        :m_color(_color),
         m_dScatter(_dScatter)
    {}
    
    /* Returns the scattered ray at the intersection point. */
    virtual ScatteredRay scatter(const Intersect &_hit, const Ray &_ray, RandomGen &_randomGen) const override {
        auto normal = (_hit.m_normal + randomUnitSphere(_randomGen) * m_dScatter).normalized();
        auto reflectedRay = Ray(_hit.m_position, reflect(_ray.m_direction, normal));
    
        return ScatteredRay(reflectedRay, m_color, Color());
    }

 private:
    Color           m_color;
    double          m_dScatter;
};


// glass material
class Glass : public Material
{
 public:
    Glass(const Color &_color, double _dScatter, double _dIndexOfRefraction)
        :m_color(_color),
         m_dScatter(_dScatter),
         m_dIndexOfRefraction(_dIndexOfRefraction)
    {}
    
    /* Returns the scattered ray at the intersection point. */
    virtual ScatteredRay scatter(const Intersect &_hit, const Ray &_ray, RandomGen &_randomGen) const override {
        auto normal = (_hit.m_normal + randomUnitSphere(_randomGen) * m_dScatter).normalized();
        double dEtaiOverEtat = 1.0/m_dIndexOfRefraction;
        
        if (_hit.m_bInside == true) {
            normal = -normal;
            dEtaiOverEtat = m_dIndexOfRefraction;
        }
        
        auto refractedRay = Ray(_hit.m_position, refract(_ray.m_direction, normal, dEtaiOverEtat, _randomGen));
        return ScatteredRay(refractedRay, m_color, Color());
    }

 private:
    Color           m_color;
    double          m_dScatter;
    double          m_dIndexOfRefraction;
};


/* Transform wrapper for shapes */
class Transform        : public Shape
{
 public:
    Transform()
    {}
    
    Transform(const std::shared_ptr<Shape> &_pTarget, const Axis &_axis, const Vec &_origin)
        :m_pTarget(_pTarget),
         m_origin(_origin),
         m_axis(_axis)
    {}
    
    /* Returns the material used for rendering, etc. */
    const Material *material() const override {
        return m_pTarget->material();
    }
    
    /* Returns the shape / ray intersect (calculates all hit properties). */
    virtual Intersect intersect(const Ray &_ray) const override {
        auto br = Ray(m_axis.translateTo(_ray.m_origin - m_origin),
                      m_axis.translateTo(_ray.m_direction));
        
        auto hit = m_pTarget->intersect(br);
        hit.m_normal = m_axis.translateFrom(hit.m_normal);
        hit.m_position = m_axis.translateFrom(hit.m_position) + m_origin;
        
        return hit;
    }
     
 private:
    std::shared_ptr<Shape>      m_pTarget;
    Axis                        m_axis;
    Vec                         m_origin;
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


/* Raytracing job (block of pixels on output image) */
class PixelJob  : public Job
{
 public:
    PixelJob(std::unique_ptr<OutputImage> &&_pOutput,
             std::unique_ptr<Viewport> &&_pView,
             const std::shared_ptr<Scene> &_pScene,
             int _iRaysPerPixel, int _iMaxDepth)
        :m_pOutput(std::move(_pOutput)),
         m_pView(std::move(_pView)),
         m_pScene(_pScene),
         m_iRaysPerPixel(_iRaysPerPixel),
         m_iMaxDepth(_iMaxDepth)
    {}
    
    void run() const
    {
        thread_local static RandomGen generator;
        renderImage(m_pOutput, m_pView, m_pScene, generator, m_iRaysPerPixel, m_iMaxDepth);
    }

 private:
    std::unique_ptr<OutputImage>        m_pOutput;
    std::unique_ptr<Viewport>           m_pView;
    std::shared_ptr<Scene>              m_pScene;
    int                                 m_iRaysPerPixel;
    int                                 m_iMaxDepth;
};


void renderFrame(OutputImageBuffer &_image, const ViewportScreen &_view,
                 const std::shared_ptr<Scene> &_pScene,
                 int _iMaxTraceDepth,
                 int _iSamplesPerPixel,
                 int _iNumWorkers)
{
    // create jobs (chop output image into smaller blocks)
    auto pJobQueue = std::make_shared<JobQueue>();
    int iPixelBlockSize = 32;
    int width = _image.width();
    int height = _image.height();

    for (int j = 0; j < height; j += iPixelBlockSize) {
        int iBlockHeight = iPixelBlockSize;
        if (iBlockHeight > height - j) {
            iBlockHeight = height - j;
        }

        for (int i = 0; i < width; i += iPixelBlockSize) {
            int iBlockWidth = iPixelBlockSize;
            if (iBlockWidth > width - i) {
                iBlockWidth = width - i;
            }
            
           pJobQueue->push(std::make_unique<PixelJob>(std::make_unique<OutputImageBlock>(_image, i, j, iBlockWidth, iBlockHeight),
                                                      std::make_unique<ViewportBlock>(_view, i, j),
                                                      _pScene,
                                                      _iSamplesPerPixel, _iMaxTraceDepth));
        }
    }
    
    // start workers
    std::vector<std::unique_ptr<Worker>> workers;
    for (int i = 0; i < _iNumWorkers; i++) {
        workers.push_back(std::make_unique<Worker>(pJobQueue, 4));
    }

    // wait for all workers to finish
    for (;;)
    {
        int numJobs = (int)pJobQueue->size();
        for (const auto &pWorker : workers) {
            numJobs += pWorker->activeJobs();
        }
        
        if (numJobs == 0) {
            break;
        }
        else {
            printf("%d\n", numJobs);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    // stats
    printf("Frame done.");
}

int raytracer()
{
    int width = 1920;
    int height = 1200;
    int fov = 60;
    int numWorkers = 16;
    int samplesPerPixel = 4096;
    int maxTraceDepth = 32;

    // init
    HighPrecisionScopeTimer timer;
    OutputImageBuffer image(width, height);
    ViewportScreen view(width, height, fov);
    auto pScene = std::make_shared<SimpleScene>();
    auto pCamera = std::make_shared<Camera>(Vec(0, 20, 80), Vec(0, 1, 0), Vec(0, 0, -10));
    view.setCamera(pCamera);
    
    // create scene
    auto pDiffuse1 = std::make_shared<DiffuseCheckered>(Color(1.0, 0.8, 0.1), Color(1.0, 0.2, 0.1), 8);
    auto pDiffuse2 = std::make_shared<DiffuseCheckered>(Color(1.0, 1.0, 0.1), Color(0.2, 0.2, 0.2), 8);
    auto pGlass1 = std::make_shared<Glass>(Color(1.0, 1.0, 1.0), 0.01, 1.5);
    auto pMetal1 = std::make_shared<Metal>(Color(1.0, 1.0, 1.0), 0.05);

    pScene->addShape(std::make_shared<Plane>(pDiffuse1));
    pScene->addShape(std::make_shared<Transform>(std::make_shared<Sphere>(15, pGlass1), axisIdentity(), Vec(0, 15, 10)));
    pScene->addShape(std::make_shared<Transform>(std::make_shared<Sphere>(15, pDiffuse2), axisIdentity(), Vec(30, 15, -20)));
    pScene->addShape(std::make_shared<Transform>(std::make_shared<Sphere>(15, pDiffuse2), axisEulerZYX(0, 0.5, 0), Vec(-30, 15, -20)));
    pScene->addShape(std::make_shared<Transform>(std::make_shared<Box>(Vec(5, 10, 5), pDiffuse2), axisEulerZYX(0, 1, 0), Vec(10, 5, 30)));
    pScene->addShape(std::make_shared<Transform>(std::make_shared<Box>(Vec(5, 10, 5), pMetal1), axisEulerZYX(0, 0.2, 0), Vec(-10, 5, 30)));
    pScene->addShape(std::make_shared<Transform>(std::make_shared<Box>(Vec(5, 10, 5), pGlass1), axisEulerZYX(0, 0.5, 0), Vec(0, 5, 30)));

    /*
    pScene->addShape(std::make_shared<Disc>(Vec(0, 40, -100), Vec(0, -1, 0), 40, std::make_unique<DiffuseCheckered>(Color(1.0, 0.8, 0.1), Color(1.0, 0.2, 0.1), 8)));
    pScene->addShape(std::make_shared<Rectangle>(Vec(-40, 30, -120), Vec(0, -1, 0), 10, 10, std::make_unique<DiffuseCheckered>(Color(1.0, 0.8, 0.1), Color(1.0, 0.2, 0.1), 8)));
    pScene->addShape(std::make_shared<Sphere>(Vec(0, 4, -35), 4, std::make_unique<DiffuseCheckered>(Color(1.0, 1.0, 1.0), Color(0.4, 0.4, 0.4), 16)));
    pScene->addShape(std::make_shared<Sphere>(Vec(-10, 3, -35), 5, std::make_unique<Diffuse>(Color(0.2, 1.0, 0.2))));
    pScene->addShape(std::make_shared<Sphere>(Vec(10, -4, -15), 3, std::make_unique<Diffuse>(Color(0.2, 1.0, 0.2))));
    pScene->addShape(std::make_shared<Sphere>(Vec(10, 5, -35), 5, std::make_unique<Metal>(Color(0.3, 0.3, 1.0), 0.05)));
    pScene->addShape(std::make_shared<Sphere>(Vec(-8, -4, -20), 3, std::make_unique<Metal>(Color(1.0, 1.0, 1.0), 0.05)));
    pScene->addShape(std::make_shared<Sphere>(Vec(0, -2, -18), 4, std::make_unique<Glass>(Color(1.0, 1.0, 1.0), 0.01, 1.5)));
    pScene->addShape(std::make_shared<Sphere>(Vec(-2, -6, -15), 1.5, std::make_unique<Glass>(Color(1.0, 1.0, 1.0), 0.01, 1.5)));
    pScene->addShape(std::make_shared<Sphere>(Vec(-20, 40, -20), 10, std::make_unique<Light>(Color(10.0, 10.0, 10.0))));
    pScene->addShape(std::make_shared<Sphere>(Vec(5, 40, -15), 5, std::make_unique<Light>(Color(20.0, 20.0, 20.0))));
    pScene->addShape(std::make_shared<Box>(Vec(3, -3, -12), Vec(2, 3, 3), std::make_unique<DiffuseCheckered>(Color(1.0, 1.0, 1.0), Color(0.4, 0.4, 0.4), 16)));
     */
    
    // render frame
    renderFrame(image, view, pScene, maxTraceDepth, samplesPerPixel, numWorkers);
    
    // write output
    writeJpegFile("raytraced.jpeg", image.width(), image.height(), image.data(), 100);
    
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




int main()
{
    raytracer();
    //mandlebrot();
    return 0;
}
