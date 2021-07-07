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
        Viewport(int _iWidth, int _iHeight)
            :m_iWidth(_iWidth),
             m_iHeight(_iHeight),
             m_fViewAspect((float)_iWidth / (float)_iHeight)
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
        
     private:
        Camera                      *m_pCamera;
        const int                   m_iWidth;
        const int                   m_iHeight;
        const float                 m_fViewAspect;
    };

    

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_VIEWPORT_H

