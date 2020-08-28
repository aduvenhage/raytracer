
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

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_VEC3_H

