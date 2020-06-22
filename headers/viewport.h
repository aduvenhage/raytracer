#ifndef LIBS_HEADER_VIEWPORT_H
#define LIBS_HEADER_VIEWPORT_H

#include "constants.h"
#include "ray.h"
#include "vec3.h"


namespace LNF
{
    // TODO: add camera origin and orientation
    class Viewport
    {
     public:
        Viewport(int _iWidth, int _iHeight, int _iFovDeg)
            :m_iWidth(_iWidth),
             m_iHeight(_iHeight),
             m_dViewAspect(0),
             m_dScale(0)
        {
            m_dScale = tan(deg2rad(_iFovDeg * 0.5));
            m_dViewAspect = (double)m_iWidth / (double)m_iHeight;
        }
        
        Ray getRay(int _iX, int _iY) {
            double x = (2 * (_iX + 0.5) / (double)m_iWidth - 1) * m_dViewAspect * m_dScale;
            double y = (1 - 2 * (_iY + 0.5) / (double)m_iHeight) * m_dScale;
                
            return Ray(Vec(), Vec(x, y, -1).normalize());
        }
        
     protected:
        int     m_iWidth;
        int     m_iHeight;
        double  m_dViewAspect;
        double  m_dScale;
    };



};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_VIEWPORT_H

