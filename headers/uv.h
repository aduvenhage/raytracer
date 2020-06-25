#ifndef LIBS_HEADER_UV_H
#define LIBS_HEADER_UV_H

#include "color.h"


namespace LNF
{
    /* UV/Texture coordinates */
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


    /* Diffuse material/texture base class */
    class Material
    {
     public:
        virtual ~Material() = default;
        
        /* Returns the diffuse color at the given surface position */
        virtual Color color(const Uv &_uv) const = 0;
        
        /* Returns material property [0..1] */
        virtual double reflection(const Uv &_uv) = 0;
        
        /* Returns material property [0..1] */
        virtual double transparancy(const Uv &_uv) = 0;
        
        /* Returns material property */
        virtual double indexOfRefraction() = 0;

    };

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_UV_H

