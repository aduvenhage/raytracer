#ifndef LIBS_HEADER_SDF_H
#define LIBS_HEADER_SDF_H

#include "constants.h"
#include "vec3.h"


namespace LNF
{
    float sdfSphere(const Vec &_p, float _fRadius) {
        const float scale = 1.0;
        return (_p.size() - _fRadius) * scale;
    }


    float sdfSphereDeformed(const Vec &_p, float _fRadius, float _fWaveHeight) {
        const float scale = 1.0;
        return (_p.size() - _fRadius + _fWaveHeight * sin(_p.x()/2) * sin(_p.y()/2) * sin(_p.z()/2)) * scale;
    }


    float sdfSwirl(const Vec &_p, float _fRadius, float _fWaveHeight) {
        const float scale = 1.0;
        auto axis = axisEulerZYX(0, _p.y()/6, 0);
        auto pr = axis.rotateFrom(_p);
        
        return (_p.size() - _fRadius - _fWaveHeight * sin(pr.x()/2) * sin(pr.z()/2)) * scale;
    }


    /*

    float sdfBubbles(const Vec &_p, double _dAngleY) {
        float sdf = 0;
        float k = 3;
        std::normal_distribution<float> p(0, 1);
        
        int n = 16;
        for (int i = 0; i < n; i ++) {
            float t = (float)i/n;
            Vec origin(2*sin(t * M_PI * 4 + _dAngleY), (t - 0.5) * 30, 1.5*cos(t * M_PI * 4 + _dAngleY));
            sdf += exp(-k * dfSphere(_p, origin,  frac(t/0.3) * 1 + 0.1));
        }
        
        return -log(sdf);
    }
    */

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_SDF_H

