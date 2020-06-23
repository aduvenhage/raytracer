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

using namespace LNF;




int raytracer()
{
    HighPrecisionScopeTimer timer;
    int width = 1280;
    int height = 960;
    auto view = Viewport(width, height, 60);
    
    std::vector<unsigned char> image(width * height * 3);
    std::vector<Sphere> spheres = {
        Sphere(Vec(0, 5, -40), 10, Color(1, 0.8, 0.8), 0, 0.8),
        Sphere(Vec(-8, 6, -32), 5, Color(0.8, 1, 0.8), 0, 0.3),
        Sphere(Vec(10, 5, -25), 5, Color(0.8, 0.8, 1), 0, 0.3)
    };
    
    int ipx = 0;
    unsigned char *pImage = image.data();
    
    for (auto j = 0; j < height; j++) {
        for (auto i = 0; i < width; i++) {
            auto ray = view.getRay(i, j);
            auto color = LNF::trace(ray, spheres, 50);
            
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
