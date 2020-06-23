
#ifndef LIBS_HEADER_VEC3_H
#define LIBS_HEADER_VEC3_H

#include "constants.h"


namespace LNF
{
    /**
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
        
        Vec(double _dX, double _dY, double _dZ)
            :m_dX(_dX),
             m_dY(_dY),
             m_dZ(_dZ)
        {}
        
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
        
        /// dot product
        double operator*(const Vec &_vec) const {
            return m_dX * _vec.m_dX +
                   m_dY * _vec.m_dY +
                   m_dZ * _vec.m_dZ ;
        }

        Vec operator*(double _dScale) const {
            return Vec(m_dX*_dScale,
                       m_dY*_dScale,
                       m_dZ*_dScale);
        }
        
        Vec normalize() const {
            double r = size();
            return Vec(m_dX / r,
                       m_dY / r,
                       m_dZ / r);
        }
        
        Vec inverse() const {
            return Vec(-1 * m_dX, -1 * m_dY, -1 * m_dZ);
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
    
};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_VEC3_H

