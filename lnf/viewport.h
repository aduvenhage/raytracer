#ifndef LIBS_HEADER_VIEWPORT_H
#define LIBS_HEADER_VIEWPORT_H

#include "camera.h"
#include "constants.h"
#include "ray.h"
#include "vec3.h"

#include <random>



namespace LNF
{
    /* viewport base class */
    class Viewport
    {
     public:
        virtual ~Viewport() = default;
        
        virtual Ray getRay(int _iX, int _iY, RandomGen &_generator, int _iPerPixelRayIndex) const = 0;
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
        
        Ray getRay(int _iX, int _iY, RandomGen &_generator, int _iPerPixelRayIndex) const override {
            std::uniform_real_distribution<float> pixelDist(-0.49999, 0.49999);

            float x = (2 * (_iX + pixelDist(_generator)) / (float)m_iWidth - 1) * m_fViewAspect * m_fScale;
            float y = (1 - 2 * (_iY + pixelDist(_generator)) / (float)m_iHeight) * m_fScale;
                
            if (m_pCamera == nullptr) {
                return Ray(Vec(), Vec(x, y, -1).normalized());
            }
            else if (m_pCamera->aperture() < 0.0001) {
                return Ray(m_pCamera->origin(),
                           m_pCamera->rotateFrom(Vec(-x, y, 1).normalized()));
            }
            else {
                auto rayOrigin = randomUnitDisc(_generator) * m_pCamera->aperture() * 0.5;
                auto rayFocus = Vec(-x, y, 1).normalized() * m_pCamera->focusDistance();
                
                // transform from camera to world
                rayOrigin = m_pCamera->transformFrom(rayOrigin);
                rayFocus = m_pCamera->transformFrom(rayFocus);
                
                // create ray
                return Ray(rayOrigin, (rayFocus - rayOrigin).normalized());
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
        
        Ray getRay(int _iX, int _iY, RandomGen &_generator, int _iPerPixelRayIndex) const override {
            return m_pViewport->getRay(_iX + m_iStartX, _iY + m_iStartY, _generator, _iPerPixelRayIndex);
        }
        
     protected:
        const Viewport     *m_pViewport;
        const int          m_iStartX;
        const int          m_iStartY;
    };



};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_VIEWPORT_H

