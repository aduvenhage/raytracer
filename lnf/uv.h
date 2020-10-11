#ifndef LIBS_HEADER_UV_H
#define LIBS_HEADER_UV_H


namespace LNF
{
    /* UV/Texture coordinates. [u, v] = [0..1, 0..1]*/
    struct Uv
    {
        Uv()
            :m_uv{}
        {}
        
        Uv(float _fU, float _fV)
            :m_uv{_fU, _fV}
        {}
        
        Uv(const Uv &) = default;
        Uv(Uv &&) = default;
        Uv(Uv &) = default;
        
        float u() const {return m_uv[0];}
        float &u() {return m_uv[0];}
        float v() const {return m_uv[1];}
        float &v() {return m_uv[1];}
        
        Uv &operator=(const Uv &) = default;
        Uv &operator=(Uv &&) = default;

        /// set color values to 0 if smaller and 1 if larger.
        Uv &clamp() {
            m_uv[0] = ::clamp(m_uv[0], 0.0f, 1.0f);
            m_uv[1] = ::clamp(m_uv[1], 0.0f, 1.0f);
            return *this;
        }
        
        /// wrap color values and keep range [0...1]
        Uv &wrap() {
            m_uv[0] -= floor(m_uv[0]);
            m_uv[1] -= floor(m_uv[1]);
            return *this;
        }
        
        float m_uv[2];
    };

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_UV_H

