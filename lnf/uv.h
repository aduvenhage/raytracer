#ifndef LIBS_HEADER_UV_H
#define LIBS_HEADER_UV_H


#include "vec3.h"
#include "constants.h"


namespace LNF
{
    /* UV/Texture coordinates. [u, v] = [0..1, 0..1]*/
    struct Uv
    {
        Uv() noexcept
            :m_uv{}
        {}
        
        Uv(float _fU, float _fV) noexcept
            :m_uv{_fU, _fV}
        {}
        
        Uv(const Uv &) noexcept = default;
        Uv(Uv &&) noexcept = default;
        //Uv(Uv &) noexcept = default;
        
        float u() const {return m_uv[0];}
        float &u() {return m_uv[0];}
        float v() const {return m_uv[1];}
        float &v() {return m_uv[1];}
        
        Uv &operator=(const Uv &) noexcept = default;
        Uv &operator=(Uv &&) noexcept = default;
        
        Uv operator+(const Uv &_uv) const {
            return Uv(m_uv[0] + _uv.m_uv[0], m_uv[1] + _uv.m_uv[1]);
        }

        Uv operator-(const Uv &_uv) const {
            return Uv(m_uv[0] - _uv.m_uv[0], m_uv[1] - _uv.m_uv[1]);
        }

        Uv operator*(float _fScale) const {
            return Uv(m_uv[0] * _fScale, m_uv[1] * _fScale);
        }

        Uv operator/(float _fScale) const {
            return Uv(m_uv[0] / _fScale, m_uv[1] / _fScale);
        }

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
    
    
    Uv operator*(float _fScale, const Uv &_uv) {
        return Uv(_uv.u() * _fScale, _uv.v() * _fScale);
    }


    /* Calculates UV coordinates from vector and radius. */
    Uv getSphericalUv(const Vec &_p, float _fRadius) {
        const float phi = atan2(_p.z(), _p.x());
        const float theta = acos(_p.y() / _fRadius);
        return Uv(phi / LNF::pi * 0.5f + 0.5f, theta / LNF::pi + 0.5f);
    }


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_UV_H

