#pragma once

#include "constants.h"


namespace CORE
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
        int                   m_iWidth;
        int                   m_iHeight;
        float                 m_fViewAspect;
    };

    

};  // namespace CORE

