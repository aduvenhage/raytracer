#ifndef LIBS_HEADER_UV_H
#define LIBS_HEADER_UV_H


namespace LNF
{
    /* UV/Texture coordinates. [u, v] = [0..1, 0..1]*/
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
        
        Uv(const Uv &) = default;
        Uv(Uv &&) = default;
        Uv(Uv &) = default;
        
        Uv &operator=(const Uv &) = default;
        Uv &operator=(Uv &&) = default;

        /// set color values to 0 if smaller and 1 if larger.
        Uv &clamp() {
            if (m_dU > 1.0) m_dU = 1.0;
            else if (m_dV < 0) m_dV = 0.0;
            
            if (m_dU > 1.0) m_dU = 1.0;
            else if (m_dV < 0) m_dV = 0.0;
            
            return *this;
        }
        
        /// wrap color values and keep range [0...1]
        Uv &wrap() {
            m_dU -= floor(m_dU);
            m_dV -= floor(m_dV);
            return *this;
        }
        
        double  m_dU;
        double  m_dV;
    };

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_UV_H

