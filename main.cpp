#include "headers/color.h"
#include "headers/constants.h"
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


// simple scene with a linear search for object hits
class SimpleScene   : public Scene
{
 public:
    virtual Intersect hit(const Ray &_ray) const override {
        double dOnRayMin = _ray.m_dMaxDist;
        Shape *pHitShape = nullptr;
        
        for (auto &pShape : m_shapes) {
            double dPositionOnRay = pShape->intersect(_ray);
            if ( (dPositionOnRay < dOnRayMin) && (dPositionOnRay > _ray.m_dMinDist) )
            {
                pHitShape = pShape.get();
                dOnRayMin = dPositionOnRay;
            }
        }
        
        if ( (pHitShape != nullptr) &&
             (dOnRayMin > 0) )
        {
            return Intersect(pHitShape, _ray, dOnRayMin);
        }
        
        return Intersect();
    }
    
    virtual Color missColor(const Ray &_ray) const override {
        double shift = _ray.m_direction.m_dY * _ray.m_direction.m_dY * 0.3 + 0.3;
        return Color(shift, shift, 0.6);
    }
    
    void addShape(const std::shared_ptr<Shape> &_pShape) {
        m_shapes.push_back(_pShape);
    }
    
 protected:
    std::vector<std::shared_ptr<Shape>>   m_shapes;
};


/* Raytracing job (block of pixels on output image) */
class PixelJob
{
 public:
    PixelJob(std::unique_ptr<OutputImage> &_pOutput, std::unique_ptr<Viewport> &_pView)
        :m_pOutput(std::move(_pOutput)),
         m_pView(std::move(_pView))
    {}
    
    bool isValid() const {
        return m_pOutput != nullptr;
    }
    
    void run(const std::shared_ptr<Scene> &_scene, RandomGen &_generator, int _iRaysPerPixel, int _iMaxDepth) const
    {
        renderImage(m_pOutput, m_pView, _scene, _generator, _iRaysPerPixel, _iMaxDepth);
    }

 private:
    std::unique_ptr<OutputImage>        m_pOutput;
    std::unique_ptr<Viewport>           m_pView;
};


std::vector<std::unique_ptr<PixelJob>>  jobs;
std::mutex                              jobMutex;


// take one job from back of job list
std::unique_ptr<PixelJob> getJob() {
    std::lock_guard<std::mutex> lock(jobMutex);
    if (jobs.empty() == false) {
        auto job = std::move(jobs.back());
        jobs.pop_back();
        return job;
    }
    
    return nullptr;
}


/* Ratracing worker thread */
class PixelWorker
{
 public:
    PixelWorker(const std::shared_ptr<Scene> &_scene,
                int _iSamplesPerPixel,
                int _iMaxTraceDepth)
        :m_scene(_scene),
         m_iSamplesPerPixel(_iSamplesPerPixel),
         m_iMaxTraceDepth(_iMaxTraceDepth),
         m_bFinished(false)
    {
        m_thread = std::thread(&PixelWorker::run, this);
    }
    
    ~PixelWorker() {
        m_thread.join();
    }

    bool finished() const {
        return m_bFinished;
    }
    
 private:
    // thread entry point
    void run() {
        // run thread until all jobs have been executed
        for (;;)
        {
            auto pJob = getJob();
            if (pJob != nullptr) {
                pJob->run(m_scene, m_generator, m_iSamplesPerPixel, m_iMaxTraceDepth);
            }
            else {
                m_bFinished = true;
                break;
            }
        }
    }
    
 protected:
    RandomGen                                   m_generator;
    std::shared_ptr<Scene>                      m_scene;
    const int                                   m_iSamplesPerPixel;
    const int                                   m_iMaxTraceDepth;
    std::thread                                 m_thread;
    std::atomic<bool>                           m_bFinished;
};

std::vector<std::unique_ptr<PixelWorker>> workers;



int raytracer()
{
    int width = 1920;
    int height = 1080;
    int fov = 60;
    int numWorkers = 16;
    int samplesPerPixel = 16;
    int maxTraceDepth = 32;

    // init
    HighPrecisionScopeTimer timer;
    OutputImageBuffer image(width, height);
    ViewportScreen view(width, height, fov);
    auto pScene = std::make_shared<SimpleScene>();
    
    // create scene
    pScene->addShape(std::make_shared<Plane>(Vec(0, -8, 0), Vec(0, 1, 0), std::make_unique<DiffuseCheckered>(Color(1.0, 0.8, 0.1), Color(1.0, 0.2, 0.1), 8)));
    pScene->addShape(std::make_shared<Sphere>(Vec(10, -4, -25), 3, std::make_unique<DiffuseCheckered>(Color(1.0, 1.0, 1.0), Color(0.4, 0.4, 0.4), 16)));
    pScene->addShape(std::make_shared<Sphere>(Vec(0, 4, -35), 4, std::make_unique<DiffuseCheckered>(Color(1.0, 1.0, 1.0), Color(0.4, 0.4, 0.4), 16)));
    pScene->addShape(std::make_shared<Sphere>(Vec(-10, 3, -35), 5, std::make_unique<Diffuse>(Color(0.2, 1.0, 0.2))));
    pScene->addShape(std::make_shared<Sphere>(Vec(10, -4, -15), 3, std::make_unique<Diffuse>(Color(0.2, 1.0, 0.2))));
    pScene->addShape(std::make_shared<Sphere>(Vec(10, 5, -35), 5, std::make_unique<Metal>(Color(0.3, 0.3, 1.0), 0.05)));
    pScene->addShape(std::make_shared<Sphere>(Vec(-8, -4, -20), 3, std::make_unique<Metal>(Color(1.0, 1.0, 1.0), 0.05)));
    pScene->addShape(std::make_shared<Sphere>(Vec(0, -2, -18), 4, std::make_unique<Glass>(Color(1.0, 1.0, 1.0), 0.01, 1.5)));
    pScene->addShape(std::make_shared<Sphere>(Vec(-2, -6, -15), 1.5, std::make_unique<Glass>(Color(1.0, 1.0, 1.0), 0.01, 1.5)));
    pScene->addShape(std::make_shared<Sphere>(Vec(-20, 40, -20), 10, std::make_unique<Light>(Color(10.0, 10.0, 10.0))));
    
    // create jobs (chop output image into smaller blocks)
    int iPixelBlockSize = 32;
    
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
            
            std::unique_ptr<OutputImage> pImage = std::make_unique<OutputImageBlock>(image, i, j, iBlockWidth, iBlockHeight);
            std::unique_ptr<Viewport> pView = std::make_unique<ViewportBlock>(view, i, j);

            jobs.push_back(std::make_unique<PixelJob>(pImage, pView));
        }
    }
    
    // start workers
    std::unique_lock<std::mutex> lock(jobMutex);
    for (int i = 0; i < numWorkers; i++) {
        workers.push_back(std::make_unique<PixelWorker>(pScene, samplesPerPixel, maxTraceDepth));
    }
    
    lock.unlock();
    
    // wait for all workers to finish
    while (workers.empty() == false) {
        auto &pWorker = workers.back();
        if ( (pWorker == nullptr) ||
             (pWorker->finished() == true) ) {
            workers.pop_back();
        }
             
        printf("%d\n", (int)jobs.size());
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // write output
    writeJpegFile("raytraced.jpeg", image.width(), image.height(), image.data(), 100);
    
    // stats
    printf("num rays = %lu\n", (unsigned long)uTraceCount);
    
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
