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
        
        Uv(float _dU, float _dV)
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
            m_dU = ::clamp(m_dU, 0.0f, 1.0f);
            m_dV = ::clamp(m_dV, 0.0f, 1.0f);
            return *this;
        }
        
        /// wrap color values and keep range [0...1]
        Uv &wrap() {
            m_dU -= floor(m_dU);
            m_dV -= floor(m_dV);
            return *this;
        }
        
        float  m_dU;
        float  m_dV;
    };

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_UV_H

