
#ifndef LIBS_HEADER_VEC3_H
#define LIBS_HEADER_VEC3_H


#define _USE_MATH_DEFINES
#include <cmath>


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
        
        double heading() const {
            return atan2(m_dZ, m_dX);
        }
         
        double pitch() const {
            double r = sqrt(m_dX * m_dX + m_dZ * m_dZ);
            return atan2(m_dY, r);
        }
        
        double      m_dX;
        double      m_dY;
        double      m_dZ;
    };

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_VEC3_H

