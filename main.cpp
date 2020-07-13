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


std::default_random_engine generator;


Vec randomUnitCube() {
    static std::uniform_real_distribution<double> distribution(-1, 1);
    return Vec(distribution(generator), distribution(generator), distribution(generator));
}


class Diffuse : public Material
{
 public:
    Diffuse(const Color &_color)
        :m_color(_color)
    {}
    
    /* Returns the diffuse color at the given surface position */
    virtual Color color(const Intersect &_hit) const override {return m_color;}
    
    /* Returns the emitted color at the given surface position */
    virtual Color emitted(const Intersect &_hit) const override {return Color();}

    /* Returns the scattered ray at the intersection point. */
    virtual ScatteredRay scatter(const Intersect &_hit, const Ray &_ray, const std::default_random_engine &_randomGen) const override {
        auto scatteredDirection = (_hit.m_normal + randomUnitCube()*0.8).normalized();
        return ScatteredRay(Ray(_hit.m_position, scatteredDirection),
                            m_color);
    }

 private:
    Color     m_color;
};


double randomPixelCenter() {
    static std::uniform_real_distribution<double> distribution(0.45, 0.55);
    return distribution(generator);
}


int raytracer()
{
    HighPrecisionScopeTimer timer;
    int width = 1280;
    int height = 960;
    int pixeln = 64;
    auto view = Viewport(width, height, 60);
    
    std::vector<unsigned char> image(width * height * 3);
    std::vector<std::shared_ptr<Shape>> shapes{
        std::make_shared<Plane>(Vec(0, -5, 0), Vec(0, 1, 0), std::make_unique<Diffuse>(Color(0.8, 0.8, 0.8))),
        std::make_shared<Sphere>(Vec(0, 5, -40), 10, std::make_unique<Diffuse>(Color(1.0, 0.1, 0.1))),
        std::make_shared<Sphere>(Vec(-10, 3, -30), 5, std::make_unique<Diffuse>(Color(0.1, 1.0, 0.1))),
        std::make_shared<Sphere>(Vec(10, 5, -30), 5, std::make_unique<Diffuse>(Color(0.1, 0.1, 1.0))),
    };
    
    int ipx = 0;
    unsigned char *pImage = image.data();
    
    for (auto j = 0; j < height; j++) {
        for (auto i = 0; i < width; i++) {
            
            auto color = Color();
            
            for (int k = 0; k < pixeln; k++) {
                auto ray = view.getRay(i, j, randomPixelCenter(), randomPixelCenter());
                color += LNF::trace(ray, shapes, generator, 50);
            }
            
            color /= pixeln;
            
            pImage[ipx++] = (int)(255 * color.m_fRed);
            pImage[ipx++] = (int)(255 * color.m_fGreen);
            pImage[ipx++] = (int)(255 * color.m_fBlue);
        }
    }
    
    writeJpegFile("raytraced.jpeg", width, height, image.data(), 100);
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
