
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
            :m_dX(0),
             m_dY(0),
             m_dZ(0)
        {}
        
        Vec(const Vec &) = default;
        Vec(Vec &&) = default;
        Vec(Vec &) = default;

        Vec(double _dX, double _dY, double _dZ)
            :m_dX(_dX),
             m_dY(_dY),
             m_dZ(_dZ)
        {}
        
        Vec &operator=(const Vec &_vec) = default;
        Vec &operator=(Vec &&_vec) = default;

        double sizeSqr() const {
            return m_dX * m_dX + m_dY * m_dY + m_dZ * m_dZ;
        }
        
        double size() const {
            return sqrt(sizeSqr());
        }
        
        Vec operator+(const Vec &_vec) const {
            return Vec(m_dX + _vec.m_dX,
                       m_dY + _vec.m_dY,
                       m_dZ + _vec.m_dZ);
        }
        
        Vec operator-(const Vec &_vec) const {
            return Vec(m_dX - _vec.m_dX,
                       m_dY - _vec.m_dY,
                       m_dZ - _vec.m_dZ);
        }
        
        Vec operator-() const {
            return inverse();
        }
        
        Vec &operator-=(const Vec &_vec) {
            m_dX -= _vec.m_dX;
            m_dY -= _vec.m_dY;
            m_dZ -= _vec.m_dZ;
            return *this;
        }
        
        Vec &operator+=(const Vec &_vec) {
            m_dX += _vec.m_dX;
            m_dY += _vec.m_dY;
            m_dZ += _vec.m_dZ;
            return *this;
        }
        
        // dot product
        double operator*(const Vec &_vec) const {
            return m_dX * _vec.m_dX +
                   m_dY * _vec.m_dY +
                   m_dZ * _vec.m_dZ ;
        }

        Vec operator*(double _dScale) const {
            return Vec(m_dX * _dScale,
                       m_dY * _dScale,
                       m_dZ * _dScale);
        }
        
        Vec operator/(double _dScale) const {
            return Vec(m_dX / _dScale,
                       m_dY / _dScale,
                       m_dZ / _dScale);
        }
        
        Vec &operator*=(double _dScale) {
            m_dX *= _dScale;
            m_dY *= _dScale;
            m_dZ *= _dScale;
            return *this;
        }
        
        Vec &operator/=(double _dScale) {
            m_dX /= _dScale;
            m_dY /= _dScale;
            m_dZ /= _dScale;
            return *this;
        }

        Vec normalized() const {
            double r = size();
            return Vec(m_dX / r,
                       m_dY / r,
                       m_dZ / r);
        }
        
        Vec inverse() const {
            return Vec(-m_dX, -m_dY, -m_dZ);
        }
        
        // per element abs() -- not the same as size
        Vec abs() const {
            return Vec(fabs(m_dX), fabs(m_dY), fabs(m_dZ));
        }
        
        double      m_dX;
        double      m_dY;
        double      m_dZ;
    };


    inline Vec operator*(double _dScale, const Vec &_vec) {
        return Vec(_vec.m_dX*_dScale,
                   _vec.m_dY*_dScale,
                   _vec.m_dZ*_dScale);
    }


    inline Vec crossProduct(const Vec &_a, const Vec &_b) {
        return Vec(_a.m_dY * _b.m_dZ - _a.m_dZ * _b.m_dY,
                   _a.m_dZ * _b.m_dX - _a.m_dX * _b.m_dZ,
                   _a.m_dX * _b.m_dY - _a.m_dY * _b.m_dX);
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
            return _vec.m_dX * m_x + _vec.m_dY * m_y + _vec.m_dZ * m_z;
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


    // returns a vector within the unit cube (-1..1, -1..1, -1..1)
    Vec randomUnitCube(RandomGen &_randomGen) {
        std::uniform_real_distribution<double> dist(-1.0, 1.0);
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
            Vec{1, 0, 0},
            Vec{0, 1, 0},
            Vec{0, 0, 1}
        };
    }


    /*
     Creates an axis set with the given orientation.
     alpha - angle around Z axis
     beta  - angle around Y axis
     gamma - angle around X axis
     */
    Axis axisEulerZYX(double _dAlpha, double _dBeta, double _dGamma) {
        const double ca = cos(_dAlpha);
        const double sa = sin(_dAlpha);
        const double cb = cos(_dBeta);
        const double sb = sin(_dBeta);
        const double cg = cos(_dGamma);
        const double sg = sin(_dGamma);

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
        auto e1 = crossProduct(_normal, Vec(0.0, 0.0, 1.0));
        if (e1.sizeSqr() < 0.0001) {
            e1 = crossProduct(_normal, Vec(0.0, 1.0, 0.0));
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

