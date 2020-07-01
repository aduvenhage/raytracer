#include "headers/color.h"
#include "headers/constants.h"
#include "headers/jpeg.h"
#include "headers/mandlebrot.h"
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

using namespace LNF;



class SolidColor : public Material
{
 public:
    SolidColor(const Color &_color, double _dReflection, double _dTransparancy)
        :m_color(_color),
         m_dReflection(_dReflection),
         m_dTransparancy(_dTransparancy)
    {}
    
    /* Returns the diffuse color at the given surface position */
    virtual Color color(const Uv &_uv) const {return m_color;}
    
    /* Returns material property [0..1] */
    virtual double reflection(const Uv &_uv) {return m_dReflection;}
    
    /* Returns material property [0..1] */
    virtual double transparancy(const Uv &_uv) {return m_dTransparancy;}
    
    /* Returns material property */
    virtual double indexOfRefraction() {return 1.5;}
    
 private:
    Color     m_color;
    double    m_dReflection;
    double    m_dTransparancy;
};


class Checkered : public Material
{
 public:
    Checkered(const Color &_color, double _dReflection, double _dTransparancy)
        :m_color(_color),
         m_dReflection(_dReflection),
         m_dTransparancy(_dTransparancy)
    {}
    
    /* Returns the diffuse color at the given surface position */
    virtual Color color(const Uv &_uv) const {
        return m_color * ((((int)(_uv.m_dU * 32) + (int)(_uv.m_dV * 32)) % 2) * 0.5 + 0.5);
    }
    
    /* Returns material property [0..1] */
    virtual double reflection(const Uv &_uv) {return m_dReflection;}
    
    /* Returns material property [0..1] */
    virtual double transparancy(const Uv &_uv) {return m_dTransparancy;}

    /* Returns material property */
    virtual double indexOfRefraction() {return 1.5;}
    
 private:
    Color     m_color;
    double    m_dReflection;
    double    m_dTransparancy;
};




class MandleBrotMat : public Material
{
 public:
    MandleBrotMat(const Color &_color, double _dReflection, double _dTransparancy)
        :m_mandlebrot(1, 1),
         m_color(_color),
         m_dReflection(_dReflection),
         m_dTransparancy(_dTransparancy)
    {}
    
    /* Returns the diffuse color at the given surface position */
    virtual Color color(const Uv &_uv) const {
        Uv uv(_uv.m_dV - 0.5, _uv.m_dU - 0.5);
        return m_color * (m_mandlebrot.value(uv.m_dU, uv.m_dV) * 0.1 + 0.1);
    }
    
    /* Returns material property [0..1] */
    virtual double reflection(const Uv &_uv) {return m_dReflection;}
    
    /* Returns material property [0..1] */
    virtual double transparancy(const Uv &_uv) {return m_dTransparancy;}

    /* Returns material property */
    virtual double indexOfRefraction() {return 1.5;}
    
 private:
    MandleBrot  m_mandlebrot;
    Color       m_color;
    double      m_dReflection;
    double      m_dTransparancy;
};


int raytracer()
{
    HighPrecisionScopeTimer timer;
    int width = 1280;
    int height = 960;
    auto view = Viewport(width, height, 60);
    
    std::vector<unsigned char> image(width * height * 3);
    std::vector<std::shared_ptr<Shape>> shapes{
        std::make_shared<Sphere>(Vec(0, 5, -40), 10, std::make_unique<Checkered>(Color(1.0, 0.1, 0.1), 0.0, 0.0)),
        std::make_shared<Sphere>(Vec(-10, 3, -30), 5, std::make_unique<SolidColor>(Color(1.0, 1.0, 1.0), 1.0, 1.0)),
        std::make_shared<Sphere>(Vec(10, 5, -30), 5, std::make_unique<MandleBrotMat>(Color(0.1, 0.1, 1.0), 0.8, 0.0)),
    };
    
    int ipx = 0;
    unsigned char *pImage = image.data();
    
    for (auto j = 0; j < height; j++) {
        for (auto i = 0; i < width; i++) {
            auto ray = view.getRay(i, j);
            auto color = LNF::trace(ray, shapes, 10);
            
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
