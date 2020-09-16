
#ifndef LIBS_HEADER_VEC3_H
#define LIBS_HEADER_VEC3_H

#include "constants.h"


namespace LNF
{
    /*
        X-Y-Z cartesian coordinate class.
     */
    struct Vec
    {
     public:
        Vec()
            :m_fX(0),
             m_fY(0),
             m_fZ(0)
        {}
        
        Vec(const Vec &) = default;
        Vec(Vec &&) = default;
        Vec(Vec &) = default;

        Vec(float _fX, float _fY, float _fZ)
            :m_fX(_fX),
             m_fY(_fY),
             m_fZ(_fZ)
        {}
        
        Vec &operator=(const Vec &_vec) = default;
        Vec &operator=(Vec &&_vec) = default;

        float sizeSqr() const {
            return m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ;
        }
        
        float size() const {
            return sqrt(sizeSqr());
        }
        
        Vec operator+(const Vec &_vec) const {
            return Vec(m_fX + _vec.m_fX,
                       m_fY + _vec.m_fY,
                       m_fZ + _vec.m_fZ);
        }
        
        Vec operator-(const Vec &_vec) const {
            return Vec(m_fX - _vec.m_fX,
                       m_fY - _vec.m_fY,
                       m_fZ - _vec.m_fZ);
        }
        
        Vec operator-() const {
            return inverse();
        }
        
        Vec &operator-=(const Vec &_vec) {
            m_fX -= _vec.m_fX;
            m_fY -= _vec.m_fY;
            m_fZ -= _vec.m_fZ;
            return *this;
        }
        
        Vec &operator+=(const Vec &_vec) {
            m_fX += _vec.m_fX;
            m_fY += _vec.m_fY;
            m_fZ += _vec.m_fZ;
            return *this;
        }
        
        // dot product
        float operator*(const Vec &_vec) const {
            return m_fX * _vec.m_fX +
                   m_fY * _vec.m_fY +
                   m_fZ * _vec.m_fZ ;
        }

        Vec operator*(float _fScale) const {
            return Vec(m_fX * _fScale,
                       m_fY * _fScale,
                       m_fZ * _fScale);
        }
        
        Vec operator/(float _fScale) const {
            return Vec(m_fX / _fScale,
                       m_fY / _fScale,
                       m_fZ / _fScale);
        }
        
        Vec &operator*=(float _fScale) {
            m_fX *= _fScale;
            m_fY *= _fScale;
            m_fZ *= _fScale;
            return *this;
        }
        
        Vec &operator/=(float _fScale) {
            m_fX /= _fScale;
            m_fY /= _fScale;
            m_fZ /= _fScale;
            return *this;
        }

        Vec normalized() const {
            float r = size();
            return Vec(m_fX / r,
                       m_fY / r,
                       m_fZ / r);
        }
        
        Vec inverse() const {
            return Vec(-m_fX, -m_fY, -m_fZ);
        }
        
        // per element abs() -- not the same as size
        Vec abs() const {
            return Vec(fabs(m_fX), fabs(m_fY), fabs(m_fZ));
        }
        
        float      m_fX;
        float      m_fY;
        float      m_fZ;
    };


    inline Vec operator*(float _fScale, const Vec &_vec) {
        return Vec(_vec.m_fX*_fScale,
                   _vec.m_fY*_fScale,
                   _vec.m_fZ*_fScale);
    }


    inline Vec crossProduct(const Vec &_a, const Vec &_b) {
        return Vec(_a.m_fY * _b.m_fZ - _a.m_fZ * _b.m_fY,
                   _a.m_fZ * _b.m_fX - _a.m_fX * _b.m_fZ,
                   _a.m_fX * _b.m_fY - _a.m_fY * _b.m_fX);
    }

    
    /*
     3 unit vector axis set
     (sort of like a matrix)
     */
    struct Axis
    {
        Axis() = default;
        Axis(const Axis &) = default;
        Axis(Axis &&) = default;
        Axis(Axis &) = default;
        
        template <typename VX, typename VY, typename VZ>
        Axis(VX &&_vx, VY &&_vy, VZ &&_vz)
            :m_x(std::forward<VX>(_vx)),
             m_y(std::forward<VY>(_vy)),
             m_z(std::forward<VZ>(_vz))
        {}

        Axis &operator=(const Axis &) = default;
        Axis &operator=(Axis &&) = default;
        
        Vec translateTo(const Vec &_vec) const {
            return Vec(_vec * m_x, _vec * m_y, _vec * m_z);
        }
                
        Vec translateFrom(const Vec &_vec) const {
            return _vec.m_fX * m_x + _vec.m_fY * m_y + _vec.m_fZ * m_z;
        }
        
        Vec     m_x;
        Vec     m_y;
        Vec     m_z;
    };
    
    
    /*
     Min/Max bounds
     */
    struct Bounds
    {
        Bounds() = default;
        Bounds(const Bounds &) = default;
        Bounds(Bounds &&) = default;
        Bounds(Bounds &) = default;
        
        template <typename VX, typename VY>
        Bounds(VX &&_min, VY &&_max)
            :m_min(std::forward<VX>(_min)),
             m_max(std::forward<VY>(_max))
        {}
        
        Vec     m_min;
        Vec     m_max;
    };


    /* test if ray is inside box or not */
    inline float *aaboxIntersect(float _t[9], const Vec &_min, const Vec &_max, const Vec &_rayOrigin, const Vec &_rayDirection) {
        // check ray origin (_t[0] = -1 outside; 1 inside)
        _t[0] = (_rayOrigin.m_fX - _min.m_fX > 0) && (_rayOrigin.m_fY - _min.m_fY > 0) && (_rayOrigin.m_fZ - _min.m_fZ > 0) &&
                (_max.m_fX - _rayOrigin.m_fX > 0) && (_max.m_fY - _rayOrigin.m_fY > 0) && (_max.m_fZ - _rayOrigin.m_fZ > 0) ?
                1.0f : -1.0f;
                
        // check ray direction (_t[9] = -1 no hit; > 0 hit; 0 no hit
        _t[1] = (_min.m_fX - _rayOrigin.m_fX) / _rayDirection.m_fX;
        _t[2] = (_max.m_fX - _rayOrigin.m_fX) / _rayDirection.m_fX;
        _t[3] = (_min.m_fY - _rayOrigin.m_fY) / _rayDirection.m_fY;
        _t[4] = (_max.m_fY - _rayOrigin.m_fY) / _rayDirection.m_fY;
        _t[5] = (_min.m_fZ - _rayOrigin.m_fZ) / _rayDirection.m_fZ;
        _t[6] = (_max.m_fZ - _rayOrigin.m_fZ) / _rayDirection.m_fZ;
        
        _t[7] = fmax(fmax(fmin(_t[1], _t[2]), fmin(_t[3], _t[4])), fmin(_t[5], _t[6]));
        _t[8] = fmin(fmin(fmax(_t[1], _t[2]), fmax(_t[3], _t[4])), fmax(_t[5], _t[6]));
        _t[9] = (_t[8] < 0.0f || _t[7] > _t[8]) ? -1.0f : _t[7];
        
        return _t;
    }


    // returns a vector within the unit cube (-1..1, -1..1, -1..1)
    Vec randomUnitCube(RandomGen &_randomGen) {
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        return Vec(dist(_randomGen),
                   dist(_randomGen),
                   dist(_randomGen));
    }

    
    // returns a vector within the unit sphere (radius of 1)
    Vec randomUnitSphere(RandomGen &_randomGen) {
        Vec ret = randomUnitCube(_randomGen);
        
        while (ret.sizeSqr() > 1) {
            ret = randomUnitCube(_randomGen);
        }
        
        return ret;
    }


    /* Creates the default axis */
    Axis axisIdentity() {
        return {
            Vec{1.0f, 0.0f, 0.0f},
            Vec{0.0f, 1.0f, 0.0f},
            Vec{0.0f, 0.0f, 1.0f}
        };
    }


    /*
     Creates an axis set with the given orientation.
     alpha - angle around Z axis
     beta  - angle around Y axis
     gamma - angle around X axis
     */
    Axis axisEulerZYX(float _fAlpha, float _fBeta, float _fGamma) {
        const float ca = cos(_fAlpha);
        const float sa = sin(_fAlpha);
        const float cb = cos(_fBeta);
        const float sb = sin(_fBeta);
        const float cg = cos(_fGamma);
        const float sg = sin(_fGamma);

        return {
            Vec{ca*cb,            sa*cb,            -sb},
            Vec{ca*sb*sg - sa*cg, sa*sb*sg + ca*cg, cb*sg},
            Vec{ca*sb*cg + sa*sg, sa*sb*cg - ca*sg, cb*cg}
        };
    }


    /*
     Creates a lookat at axis set.
     lookat - position camera is aiming at
     origin - position of camera
     up - local camera 'up' vector
     
     returns: [left, up, lookat]
     */
    Axis axisLookat(const Vec &_lookat, const Vec &_origin, const Vec &_up) {
        auto lookat = (_lookat - _origin).normalized();
        auto left = crossProduct(_up, lookat).normalized();
        auto up = crossProduct(lookat, left);
        
        return {
            left,
            up,
            lookat
        };
    }


    /*
     Creates an axis on the plane.
     returns: [e1, normal, e2]
     */
    Axis axisPlane(const Vec &_normal, const Vec &_origin) {
        auto e1 = crossProduct(_normal, Vec(0.0f, 0.0f, 1.0f));
        if (e1.sizeSqr() < 0.0001f) {
            e1 = crossProduct(_normal, Vec(0.0f, 1.0f, 0.0f));
        }
        
        e1 = e1.normalized();
        auto e2 = crossProduct(_normal, e1);
        
        return {
            e1,
            _normal,
            e2
        };
    }
    


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_VEC3_H

