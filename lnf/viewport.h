#ifndef LIBS_HEADER_VIEWPORT_H
#define LIBS_HEADER_VIEWPORT_H

#include "camera.h"
#include "constants.h"


namespace LNF
{
    /* viewport base class */
    class Viewport
    {
     public:
        // TODO: move fov to camera
        Viewport(int _iWidth, int _iHeight, int _iFovDeg)
            :m_iWidth(_iWidth),
             m_iHeight(_iHeight),
             m_fViewAspect((float)_iWidth / (float)_iHeight),
             m_fFovScale(tan(deg2rad(_iFovDeg * 0.5f)))
        {}
        
        void setCamera(Camera *_pCam) {
            m_pCamera = _pCam;
        }
        
        const Camera *camera() const {
            return m_pCamera;
        }
        
        int width() const {
            return m_iWidth;
        }
        
        int height() const {
            return m_iHeight;
        }
        
        float viewAspect() const {
            return m_fViewAspect;
        }
        
        float fovScale() const {
            return m_fFovScale;
        }
        
     protected:
        Camera                      *m_pCamera;
        const int                   m_iWidth;
        const int                   m_iHeight;
        const float                 m_fViewAspect;
        const float                 m_fFovScale;
    };

    

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_VIEWPORT_H

