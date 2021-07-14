#ifndef LIBS_HEADER_SDF_H
#define LIBS_HEADER_SDF_H

#include "constants.h"
#include "vec3.h"


namespace LNF
{
    float sdfSphere(const Vec &_p, float _fRadius) {
        return (_p.size() - _fRadius);
    }


    float sdfSphere(const Vec &_p, const Vec &_origin, float _fRadius) {
        return ((_p - _origin).size() - _fRadius);
    }


    float sdfSphereDeformed(const Vec &_p, float _fRadius, float _fWaveHeight) {
        return (_p.size() - _fRadius + _fWaveHeight * sin(_p.x()/_fRadius*8) * sin(_p.y()/_fRadius*8) * sin(_p.z()/_fRadius*8));
    }


    float sdfSwirl(const Vec &_p, float _fRadius, float _fWaveHeight) {
        auto axis = axisEulerZYX(0, _p.y()/6, 0);
        auto pr = axis.rotateFrom(_p);
        
        return (_p.size() - _fRadius - _fWaveHeight * sin(pr.x()/_fRadius*8) * sin(pr.z()/_fRadius*8));
    }


    float sdfMandle(const Vec &_p, int &_iterations) {
        float BAIL_OUT = 2.0f;
        float POWER = 8.0f;
        float PHASE = 0.0f;
        int MAX_ITERATIONS = 200;
        
        Vec z = _p;
        float dr = 1.0;
        float r = 0.0;
        int i = 0;
        for (; i < MAX_ITERATIONS; i++) {
            r = z.size();
            if (r > BAIL_OUT)
                break;
            
            // convert to polar coordinates
            float theta = acos(z.z()/r);
            float phi = atan2(z.y(), z.x()) + PHASE;
            dr =  pow(r, POWER - 1.0f) * POWER * dr + 1.0f;
            
            // scale and rotate the point
            float zr = pow(r, POWER);
            theta = theta * POWER;
            phi = phi * POWER;
            
            // convert back to cartesian coordinates
            z = zr * Vec(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
            z += _p;
        }
        
        _iterations = i;        
        return 0.1f * log(r) * r/dr;
    }


    float sdfBubbles(const Vec &_p, float _fAngleY, float _fHeight) {
        float sdf = 0;
        float k = 4;
        int n = 16;
        for (int i = 0; i < n; i ++) {
            float t = (float)i/n;
            Vec origin(0.45f*_fHeight*sin(t * LNF::pi * 4 + _fAngleY),
                       (t - 0.5f)*_fHeight,
                       0.45f*_fHeight*cos(t * LNF::pi * 4 + _fAngleY));
            
            sdf += exp(-k * sdfSphere(_p, origin, (frac(t/0.3f) + 0.1f)*_fHeight*0.1f));
        }
        
        return -log(sdf) * 0.1f;
    }


};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_SDF_H

