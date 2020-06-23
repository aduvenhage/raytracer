#ifndef LIBS_HEADER_UV_H
#define LIBS_HEADER_UV_H

#include "color.h"


namespace LNF
{
    struct Uv
    {
        Uv()
            :m_dU(0),
             m_dV(0)
        {}
        
        Uv(double _dU, double _dV)
            :m_dU(_dU),
             m_dV(_dV)
        {}
        
        double  m_dU;
        double  m_dV;
    };


    struct Material
    {
        Color   m_diffuse;
        
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_UV_H

