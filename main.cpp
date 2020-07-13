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



using namespace LNF;



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
    virtual ScatteredRay scatter(const Intersect &_hit, const Ray &_ray, std::default_random_engine &_randomGen) const override {
        auto scatteredDirection = (_hit.m_normal + randomUnitCube(_randomGen)*0.8).normalized();
        return ScatteredRay(Ray(_hit.m_position, scatteredDirection),
                            m_color);
    }

 private:
    Vec randomUnitCube(std::default_random_engine &_randomGen) const {
        return Vec(m_distribution(_randomGen),
                   m_distribution(_randomGen),
                   m_distribution(_randomGen));
    }

 private:
    Color                                          m_color;
    mutable std::uniform_real_distribution<double> m_distribution;
};


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


class PixelJob
{
 public:
    PixelJob(int _iStartX, int _iStartY, int _iWidth, int _iHeight)
        :m_iStartX(_iStartX),
         m_iStartY(_iStartY),
         m_iWidth(_iWidth),
         m_iHeight(_iHeight)
    {}
    
    void run(OutputImage &_output,
             const Viewport &_view,
             const std::vector<std::shared_ptr<Shape>> &_shapes,
             std::default_random_engine &_generator,
             int _iRaysPerPixel, int _iMaxDepth)
    {
        unsigned char *pImage = _output.data();
        const int iOutputWidth = _output.width();

        for (auto j = m_iStartY; j < m_iHeight; j++)
        {
            int ipx = j * iOutputWidth * 3;
            
            for (auto i = m_iStartY; i < m_iWidth; i++)
            {
                auto color = Color();
                
                for (int k = 0; k < _iRaysPerPixel; k++)
                {
                    auto ray = _view.getRay(i, j,
                                            m_pixelDistribution(_generator),
                                            m_pixelDistribution(_generator));
                    
                    color += LNF::trace(ray, _shapes, _generator, _iMaxDepth);
                }
                
                color /= _iRaysPerPixel;
                
                pImage[ipx++] = (int)(255 * color.m_fRed);
                pImage[ipx++] = (int)(255 * color.m_fGreen);
                pImage[ipx++] = (int)(255 * color.m_fBlue);
            }
        }
    }

 private:
    int     m_iStartX;
    int     m_iStartY;
    int     m_iWidth;
    int     m_iHeight;
    
    std::uniform_real_distribution<double>  m_pixelDistribution;
};


int raytracer()
{
    std::default_random_engine generator;
    HighPrecisionScopeTimer timer;
    OutputImage image(1280, 960);
    Viewport view(1280, 960, 60);
    PixelJob job(0, 0, 1280, 960);

    std::vector<std::shared_ptr<Shape>> shapes{
        std::make_shared<Plane>(Vec(0, -5, 0), Vec(0, 1, 0), std::make_unique<Diffuse>(Color(0.8, 0.8, 0.8))),
        std::make_shared<Sphere>(Vec(0, 5, -40), 10, std::make_unique<Diffuse>(Color(1.0, 0.1, 0.1))),
        std::make_shared<Sphere>(Vec(-10, 3, -30), 5, std::make_unique<Diffuse>(Color(0.1, 1.0, 0.1))),
        std::make_shared<Sphere>(Vec(10, 5, -30), 5, std::make_unique<Diffuse>(Color(0.1, 0.1, 1.0))),
    };
    
    job.run(image, view, shapes, generator, 64, 50);
    writeJpegFile("raytraced.jpeg", image.width(), image.height(), image.data(), 100);
    
    return 0;
}


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
