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
        return (_p.size() - _fRadius + _fWaveHeight * sin(_p.x()/_fRadius*8) * sin(_p.y()/_fRadius*8) * sin(_p.z()/_fRadius*8)) * scale;
    }


    float sdfSwirl(const Vec &_p, float _fRadius, float _fWaveHeight) {
        const float scale = 1.0;
        auto axis = axisEulerZYX(0, _p.y()/6, 0);
        auto pr = axis.rotateFrom(_p);
        
        return (_p.size() - _fRadius - _fWaveHeight * sin(pr.x()/_fRadius*8) * sin(pr.z()/_fRadius*8)) * scale;
    }

    float sdfMandle(const Vec &_p) {
        float BAIL_OUT = 2.1f;
        float POWER = 8.0f;
        float PHASE = 5;
        int ITERATIONS = 25;
        
        Vec z = _p;
        float dr = 1.0;
        float r = 0.0;
        for (int i = 0; i < ITERATIONS; i++) {
            r = z.size();
            if (r > BAIL_OUT)
                break;
            
            // convert to polar coordinates
            float theta = acos(z.z()/r);
            float phi = atan2(z.y(), z.x()) + PHASE;
            dr =  pow(r, POWER - 1.0) * POWER * dr + 1.0;
            
            // scale and rotate the point
            float zr = pow(r, POWER);
            theta = theta * POWER;
            phi = phi * POWER;
            
            // convert back to cartesian coordinates
            z = zr * Vec(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
            z += _p;
        }
        
        return 0.1 * log(r) * r/dr;
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

