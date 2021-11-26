#ifndef CORE_UV_H
#define CORE_UV_H


#include "vec3.h"
#include "constants.h"


namespace CORE
{
    /* UV/Texture coordinates. [u, v] = [0..1, 0..1]*/
    struct Uv
    {
        Uv() noexcept = default;
        
        Uv(float _fU, float _fV) noexcept
            :m_uv{_fU, _fV}
        {}
        
        float u() const {return m_uv[0];}
        float &u() {return m_uv[0];}
        float v() const {return m_uv[1];}
        float &v() {return m_uv[1];}
        
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
            m_uv[0] -= floorf(m_uv[0]);
            m_uv[1] -= floorf(m_uv[1]);
            return *this;
        }
        
        float m_uv[2] = {};
    };
    
    
    Uv operator*(float _fScale, const Uv &_uv) {
        return Uv(_uv.u() * _fScale, _uv.v() * _fScale);
    }


    /* Calculates UV coordinates from vector with unit length. */
    Uv getSphericalUv(const Vec &_n) {
        float phi = angleWrap2Pi(atan2(_n.z(), _n.x()));
        float theta = acos(clamp(_n.y(), -1.0f, 1.0f));
        return Uv(phi / pif * 0.5f, theta / pif);
    }

};  // namespace CORE

#endif  // #ifndef CORE_UV_H

