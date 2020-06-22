#ifndef LIBS_HEADER_UV_H
#define LIBS_HEADER_UV_H

#define _USE_MATH_DEFINES
#include <cmath>


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


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_UV_H

