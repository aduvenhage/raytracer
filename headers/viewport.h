#ifndef LIBS_HEADER_VIEWPORT_H
#define LIBS_HEADER_VIEWPORT_H

#include "constants.h"
#include "ray.h"
#include "vec3.h"


namespace LNF
{
    /* viewport base class */
    class Viewport
    {
     public:
        virtual ~Viewport() = default;
        
        virtual Ray getRay(int _iX, int _iY, double _dX, double _dY) const = 0;
    };


    /* fullscreen viewport */
    class ViewportScreen    : public Viewport
    {
     public:
        ViewportScreen(int _iWidth, int _iHeight, int _iFovDeg)
            :m_iWidth(_iWidth),
             m_iHeight(_iHeight),
             m_dViewAspect((double)_iWidth / (double)_iHeight),
             m_dScale(tan(deg2rad(_iFovDeg * 0.5)))
        {}
        
        Ray getRay(int _iX, int _iY, double _dX = 0.5, double _dY = 0.5) const override {
            double x = (2 * (_iX + _dX) / (double)m_iWidth - 1) * m_dViewAspect * m_dScale;
            double y = (1 - 2 * (_iY + _dY) / (double)m_iHeight) * m_dScale;
                
            return Ray(Vec(), Vec(x, y, -1).normalized());
        }
        
     protected:
        const int     m_iWidth;
        const int     m_iHeight;
        const double  m_dViewAspect;
        const double  m_dScale;
    };

    
    /* viewport sub-area */
    class ViewportBlock    : public Viewport
    {
     public:
        ViewportBlock(const ViewportScreen &_viewport, int _iStartX, int _iStartY)
            :m_viewport(_viewport),
             m_iStartX(_iStartX),
             m_iStartY(_iStartY)
        {}
        
        Ray getRay(int _iX, int _iY, double _dX = 0.5, double _dY = 0.5) const override {
            return m_viewport.getRay(_iX + m_iStartX, _iY + m_iStartY, _dX, _dY);
        }
        
     protected:
        const ViewportScreen    &m_viewport;
        const int               m_iStartX;
        const int               m_iStartY;
    };



};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_VIEWPORT_H

