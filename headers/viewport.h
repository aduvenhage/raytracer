#ifndef LIBS_HEADER_VIEWPORT_H
#define LIBS_HEADER_VIEWPORT_H

#include "camera.h"
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
        
        void setCamera(std::shared_ptr<Camera> &_pCam) {
            m_pCamera = _pCam;
        }
        
        Ray getRay(int _iX, int _iY, double _dX = 0.5, double _dY = 0.5) const override {
            double x = (2 * (_iX + _dX) / (double)m_iWidth - 1) * m_dViewAspect * m_dScale;
            double y = (1 - 2 * (_iY + _dY) / (double)m_iHeight) * m_dScale;
                
            if (m_pCamera == nullptr) {
                return Ray(Vec(x, y, -1).normalized());
            }
            else {
                return Ray(m_pCamera->m_origin,
                           m_pCamera->camera2world(Vec(-x, y, 1).normalized()));
            }
        }
        
        int width() const {
            return m_iWidth;
        }
        
        int height() const {
            return m_iHeight;
        }
        
     protected:
        std::shared_ptr<Camera>     m_pCamera;
        const int                   m_iWidth;
        const int                   m_iHeight;
        const double                m_dViewAspect;
        const double                m_dScale;
    };

    
    /* viewport sub-area */
    class ViewportBlock    : public Viewport
    {
     public:
        ViewportBlock(const std::shared_ptr<Viewport> &_pViewport, int _iStartX, int _iStartY)
            :m_pViewport(_pViewport),
             m_iStartX(_iStartX),
             m_iStartY(_iStartY)
        {}
        
        Ray getRay(int _iX, int _iY, double _dX = 0.5, double _dY = 0.5) const override {
            return m_pViewport->getRay(_iX + m_iStartX, _iY + m_iStartY, _dX, _dY);
        }
        
     protected:
        std::shared_ptr<Viewport>   m_pViewport;        // TODO: maybe drop this to a raw pointer, if it would be safe
        const int                   m_iStartX;
        const int                   m_iStartY;
    };



};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_VIEWPORT_H

