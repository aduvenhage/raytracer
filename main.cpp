#include "headers/color.h"
#include "headers/constants.h"
#include "headers/jpeg.h"
#include "headers/light.h"
#include "headers/mandlebrot.h"
#include "headers/plane.h"
#include "headers/profile.h"
#include "headers/ray.h"
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
        :m_color(_color),
         m_distribution(-1, 1)
    {}
    
    /* Returns the diffuse color at the given surface position */
    virtual Color color(const Intersect &_hit) const override {return m_color;}
    
    /* Returns the emitted color at the given surface position */
    virtual Color emitted(const Intersect &_hit) const override {return Color();}

    /* Returns the scattered ray at the intersection point. */
    virtual ScatteredRay scatter(const Intersect &_hit, const Ray &_ray, RandomGen &_randomGen) const override {
        auto scatteredDirection = (_hit.m_normal + randomUnitCube(_randomGen)*0.8).normalized();
        return ScatteredRay(Ray(_hit.m_position, scatteredDirection), color(_hit));
    }

 private:
    Vec randomUnitCube(RandomGen &_randomGen) const {
        return Vec(m_distribution(_randomGen),
                   m_distribution(_randomGen),
                   m_distribution(_randomGen));
    }

 private:
    Color                                          m_color;
    mutable std::uniform_real_distribution<double> m_distribution;
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
    MandleBrot                                      m_mandlebrot;
    Color                                           m_baseColor;
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
    Color                                           m_colorA;
    Color                                           m_colorB;
    int                                             m_iBlockSize;
};


// shiny metal material
class Metal : public Material
{
 public:
    Metal(const Color &_color, double _dScatter)
        :m_color(_color),
         m_dScatter(_dScatter),
         m_distribution(-1, 1)
    {}
    
    /* Returns the diffuse color at the given surface position */
    virtual Color color(const Intersect &_hit) const override {return m_color;}
    
    /* Returns the emitted color at the given surface position */
    virtual Color emitted(const Intersect &_hit) const override {return Color();}

    /* Returns the scattered ray at the intersection point. */
    virtual ScatteredRay scatter(const Intersect &_hit, const Ray &_ray, RandomGen &_randomGen) const override {
        auto normal = (_hit.m_normal + randomUnitCube(_randomGen) * m_dScatter).normalized();
        auto reflectedRay = Ray(_hit.m_position, reflect(_ray.m_direction, normal));
        
        return ScatteredRay(reflectedRay, color(_hit));
    }

 private:
    Vec randomUnitCube(RandomGen &_randomGen) const {
        return Vec(m_distribution(_randomGen),
                   m_distribution(_randomGen),
                   m_distribution(_randomGen));
    }
    
 private:
    Color                                          m_color;
    double                                         m_dScatter;
    mutable std::uniform_real_distribution<double> m_distribution;
};


// solid color background
class Background : public RayMiss
{
 public:
    virtual Color color(const Ray &_ray) const {
        return Color(0.8, 0.8, 0.7);
    }
};



/* Wrapper class for output image raw buffer */
class OutputImage
{
 public:
    OutputImage(int _iWidth, int _iHeight)
        :m_iWidth(_iWidth),
         m_iHeight(_iHeight),
         m_image(_iWidth * _iHeight * 3)
    {}
    
    int width() const {return m_iWidth;}
    int height() const {return m_iHeight;}
    
    unsigned char *data() {return m_image.data();}
    const unsigned char *data() const {return m_image.data();}

 private:
    int                         m_iWidth;
    int                         m_iHeight;
    std::vector<unsigned char>  m_image;
};


/* Ratracing job (block of pixels on output image) */
class PixelJob
{
 protected:
    static constexpr double    PIXEL_VARIANCE = 0.05;
    static constexpr double    MIN_RAY_DIST = 0.00001;
    static constexpr double    MAX_RAY_DIST = 100;

 public:
    PixelJob()
        :m_bValid(false),
         m_iStartX(0),
         m_iStartY(0),
         m_iWidth(0),
         m_iHeight(0)
    {}
    
    PixelJob(int _iStartX, int _iStartY, int _iWidth, int _iHeight)
        :m_bValid(true),
         m_iStartX(_iStartX),
         m_iStartY(_iStartY),
         m_iWidth(_iWidth),
         m_iHeight(_iHeight),
         m_pixelDistribution(0.5-PIXEL_VARIANCE, 0.5+PIXEL_VARIANCE)
    {}
    
    bool isValid() const {
        return m_bValid;
    }
    
    void run(OutputImage &_output,
             const Viewport &_view,
             const std::vector<std::shared_ptr<Shape>> &_shapes,
             const std::shared_ptr<RayMiss> &_missHandler,
             RandomGen &_generator,
             int _iRaysPerPixel, int _iMaxDepth) const
    {
        unsigned char *pImage = _output.data();
        const int iOutputWidth = _output.width();

        // create rays and trace them for all pixels in block
        for (auto j = 0; j < m_iHeight; j++)
        {
            int ipx = ((m_iStartY + j) * iOutputWidth + m_iStartX) * 3;
            for (auto i = 0; i < m_iWidth; i++)
            {
                auto color = Color();
                
                for (int k = 0; k < _iRaysPerPixel; k++)
                {
                    // get ray with some fuzziness around pixel center
                    auto ray = _view.getRay(i + m_iStartX, j + m_iStartY,
                                            m_pixelDistribution(_generator),
                                            m_pixelDistribution(_generator));

                    // trace ray and add color result
                    color += LNF::trace(ray, MIN_RAY_DIST, MAX_RAY_DIST, _shapes, _missHandler, _generator, _iMaxDepth);
                }
                                
                // write averaged color to output image
                color /= _iRaysPerPixel;
                pImage[ipx++] = (int)(255 * color.m_fRed);
                pImage[ipx++] = (int)(255 * color.m_fGreen);
                pImage[ipx++] = (int)(255 * color.m_fBlue);
            }
        }
    }

 private:
    bool                                            m_bValid;
    int                                             m_iStartX;
    int                                             m_iStartY;
    int                                             m_iWidth;
    int                                             m_iHeight;
    mutable std::uniform_real_distribution<double>  m_pixelDistribution;
};


std::vector<std::unique_ptr<PixelJob>>  jobs;
std::mutex                              jobMutex;


/* Ratracing worker thread */
class PixelWorker
{
 private:
    static const int SAMPLES_PER_PIXEL = 64;
    static const int MAX_DEPTH = 16;

 public:
    PixelWorker(std::vector<std::unique_ptr<PixelJob>> &_jobs, std::mutex &_jobMutex,
                OutputImage &_image,
                const Viewport &_view,
                const std::vector<std::shared_ptr<Shape>> &_scene,
                const std::shared_ptr<RayMiss> &_missHandler)
        :m_image(_image),
         m_view(_view),
         m_scene(_scene),
         m_missHandler(_missHandler),
         m_jobs(_jobs),
         m_mutex(_jobMutex),
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
    // take one job from back of job list
    std::unique_ptr<PixelJob> getJob() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_jobs.empty() == false) {
            auto job = std::move(m_jobs.back());
            m_jobs.pop_back();
            return job;
        }
        
        return nullptr;
    }
    
    // thread entry point
    void run() {
        // run thread until all jobs have been executed
        for (;;)
        {
            auto pJob = getJob();
            if (pJob != nullptr) {
                pJob->run(m_image, m_view, m_scene, m_missHandler, m_generator, SAMPLES_PER_PIXEL, MAX_DEPTH);
            }
            else {
                m_bFinished = true;
                break;
            }
        }
    }
    
 protected:
    RandomGen                                   m_generator;
    OutputImage                                 &m_image;
    const Viewport                              &m_view;
    const std::vector<std::shared_ptr<Shape>>   &m_scene;
    std::shared_ptr<RayMiss>                    m_missHandler;
    std::vector<std::unique_ptr<PixelJob>>      &m_jobs;
    std::mutex                                  &m_mutex;
    std::thread                                 m_thread;
    std::atomic<bool>                           m_bFinished;
};

std::vector<std::unique_ptr<PixelWorker>> workers;



int raytracer()
{
    int width = 1280;
    int height = 960;
    int fov = 60;
    int numWorkers = 16;

    // init
    HighPrecisionScopeTimer timer;
    OutputImage image(width, height);
    Viewport view(width, height, fov);
    
    // create scene
    std::vector<std::shared_ptr<Shape>> shapes{
        std::make_shared<Plane>(Vec(0, -8, 0), Vec(0, 1, 0), std::make_unique<DiffuseCheckered>(Color(1.0, 0.8, 0.0), Color(1.0, 0.2, 0.0), 8)),
        std::make_shared<Sphere>(Vec(0, 5, -40), 10, std::make_unique<Diffuse>(Color(1.0, 1.0, 1.0))),
        std::make_shared<Sphere>(Vec(-10, 3, -30), 5, std::make_unique<Diffuse>(Color(0.2, 1.0, 0.2))),
        std::make_shared<Sphere>(Vec(10, 5, -30), 5, std::make_unique<Metal>(Color(0.2, 0.2, 1.0), 0.02)),
    };
    
    auto background = std::make_shared<Background>();
    
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
            
            jobs.push_back(std::make_unique<PixelJob>(i, j, iBlockWidth, iBlockHeight));
        }
    }
    
    // start workers
    std::unique_lock<std::mutex> lock(jobMutex);
    for (int i = 0; i < numWorkers; i++) {
        workers.push_back(std::make_unique<PixelWorker>(jobs, jobMutex, image, view, shapes, background));
    }
    
    lock.unlock();
    
    // wait for all workers to finish
    while (workers.empty() == false) {
        auto &pWorker = workers.back();
        if ( (pWorker == nullptr) ||
             (pWorker->finished() == true) ) {
            workers.pop_back();
        }
             
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
