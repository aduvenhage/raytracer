#ifndef LIBS_HEADER_UV_H
#define LIBS_HEADER_UV_H


namespace LNF
{
    /* UV/Texture coordinates. [u, v] = [0..1, 0..1]*/
    struct Uv
    {
        Uv()
            :m_fU(0),
             m_fV(0)
        {}
        
        Uv(float _fU, float _fV)
            :m_fU(_fU),
             m_fV(_fV)
        {}
        
        Uv(const Uv &) = default;
        Uv(Uv &&) = default;
        Uv(Uv &) = default;
        
        Uv &operator=(const Uv &) = default;
        Uv &operator=(Uv &&) = default;

        /// set color values to 0 if smaller and 1 if larger.
        Uv &clamp() {
            m_fU = ::clamp(m_fU, 0.0f, 1.0f);
            m_fV = ::clamp(m_fV, 0.0f, 1.0f);
            return *this;
        }
        
        /// wrap color values and keep range [0...1]
        Uv &wrap() {
            m_fU -= floor(m_fU);
            m_fV -= floor(m_fV);
            return *this;
        }
        
        float  m_fU;
        float  m_fV;
    };

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_UV_H

