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
        
        virtual Ray getRay(int _iX, int _iY, float _fX, float _fY) const = 0;
    };


    /* fullscreen viewport */
    class ViewportScreen    : public Viewport
    {
     public:
        ViewportScreen(int _iWidth, int _iHeight, int _iFovDeg)
            :m_iWidth(_iWidth),
             m_iHeight(_iHeight),
             m_fViewAspect((float)_iWidth / (float)_iHeight),
             m_fScale(tan(deg2rad(_iFovDeg * 0.5f)))
        {}
        
        void setCamera(Camera *_pCam) {
            m_pCamera = _pCam;
        }
        
        Ray getRay(int _iX, int _iY, float _fX = 0.5f, float _fY = 0.5f) const override {
            float x = (2 * (_iX + _fX) / (float)m_iWidth - 1) * m_fViewAspect * m_fScale;
            float y = (1 - 2 * (_iY + _fY) / (float)m_iHeight) * m_fScale;
                
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
        Camera                      *m_pCamera;
        const int                   m_iWidth;
        const int                   m_iHeight;
        const float                 m_fViewAspect;
        const float                 m_fScale;
    };

    
    /* viewport sub-area */
    class ViewportBlock    : public Viewport
    {
     public:
        ViewportBlock(const Viewport *_pViewport, int _iStartX, int _iStartY)
            :m_pViewport(_pViewport),
             m_iStartX(_iStartX),
             m_iStartY(_iStartY)
        {}
        
        Ray getRay(int _iX, int _iY, float _fX = 0.5f, float _fY = 0.5f) const override {
            return m_pViewport->getRay(_iX + m_iStartX, _iY + m_iStartY, _fX, _fY);
        }
        
     protected:
        const Viewport     *m_pViewport;
        const int          m_iStartX;
        const int          m_iStartY;
    };



};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_VIEWPORT_H

