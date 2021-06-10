#ifndef LIBS_HEADER_VIEWPORT_H
#define LIBS_HEADER_VIEWPORT_H

#include "camera.h"
#include "constants.h"
#include "ray.h"
#include "vec3.h"

#include <random>
#include <atomic>



namespace LNF
{
    /* viewport base class */
    class Viewport
    {
     public:
        virtual ~Viewport() = default;
        
        virtual Ray getRay(int _iX, int _iY, RandomGen &_generator, int _iPerPixelRayIndex) const = 0;
        virtual size_t rayCount() const = 0;
    };


    /* fullscreen viewport */
    class ViewportScreen    : public Viewport
    {
     public:
        ViewportScreen(int _iWidth, int _iHeight, int _iFovDeg)
            :m_iWidth(_iWidth),
             m_iHeight(_iHeight),
             m_fViewAspect((float)_iWidth / (float)_iHeight),
             m_fScale(tan(deg2rad(_iFovDeg * 0.5f))),
             m_uRayCount(0)
        {}
        
        void setCamera(Camera *_pCam) {
            m_pCamera = _pCam;
        }
        
        Ray getRay(int _iX, int _iY, RandomGen &_generator, int _iPerPixelRayIndex) const override {
            std::uniform_real_distribution<float> pixelDist(-0.49999f, 0.49999f);

            float x = (2 * (_iX + pixelDist(_generator)) / (float)m_iWidth - 1) * m_fViewAspect * m_fScale;
            float y = (1 - 2 * (_iY + pixelDist(_generator)) / (float)m_iHeight) * m_fScale;
            m_uRayCount++;
                
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
        
        virtual size_t rayCount() const override {
            return m_uRayCount;
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
        mutable std::atomic<size_t> m_uRayCount;
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
                
        virtual size_t rayCount() const override {
            return m_pViewport->rayCount();
        }

     protected:
        const Viewport     *m_pViewport;
        const int          m_iStartX;
        const int          m_iStartY;
    };



};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_VIEWPORT_H

