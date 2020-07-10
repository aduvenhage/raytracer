#ifndef LIBS_HEADER_COLOR_H
#define LIBS_HEADER_COLOR_H

#include "constants.h"


namespace LNF
{
    /*
       R-G-B color class.
       Color components are floats [0..1].
     */
    struct Color
    {
        Color()
            :m_fRed(0),
             m_fGreen(0),
             m_fBlue(0)
        {}
        
        Color(float _fRed, float _fGreen, float _fBlue)
            :m_fRed(_fRed),
             m_fGreen(_fGreen),
             m_fBlue(_fBlue)
        {}
        
        Color operator+(const Color &_color) const {
            return Color(m_fRed + _color.m_fRed,
                         m_fGreen + _color.m_fGreen,
                         m_fBlue + _color.m_fBlue);
        }
        
        Color operator-(const Color &_color) const {
            return Color(m_fRed - _color.m_fRed,
                         m_fGreen - _color.m_fGreen,
                         m_fBlue - _color.m_fBlue);
        }
        
        Color &operator+=(const Color &_color) {
            m_fRed += _color.m_fRed;
            m_fGreen += _color.m_fGreen;
            m_fBlue += _color.m_fBlue;
            return *this;
        }
        
        Color &operator-=(const Color &_color) {
            m_fRed -= _color.m_fRed;
            m_fGreen -= _color.m_fGreen;
            m_fBlue -= _color.m_fBlue;
            return *this;
        }
        
        Color operator*(double _dScale) const {
            return Color(m_fRed * _dScale,
                         m_fGreen * _dScale,
                         m_fBlue * _dScale);
        }
        
        Color &operator*=(double _dScale) {
            m_fRed *= _dScale;
            m_fGreen *= _dScale;
            m_fBlue *= _dScale;
            return *this;
        }
        
        Color operator/(double _dScale) const {
            return Color(m_fRed / _dScale,
                         m_fGreen / _dScale,
                         m_fBlue / _dScale);
        }
        
        Color &operator/=(double _dScale) {
            m_fRed /= _dScale;
            m_fGreen /= _dScale;
            m_fBlue /= _dScale;
            return *this;
        }
        
        /// 3 element scale
        Color operator*(const Color &_color) const {
            return Color(m_fRed * _color.m_fRed,
                         m_fGreen * _color.m_fGreen,
                         m_fBlue * _color.m_fBlue);
        }
        
        /// 3 element scale
        Color &operator*=(const Color &_color) {
            m_fRed *= _color.m_fRed;
            m_fGreen *= _color.m_fGreen;
            m_fBlue *= _color.m_fBlue;
            return *this;
        }
        
        /// set color values to 0 if smaller and 1 if larger.
        Color &clamp() {
            if (m_fRed > 1.0) m_fRed = 1.0;
            else if (m_fRed < 0) m_fRed = 0.0;
            
            if (m_fGreen > 1.0) m_fGreen = 1.0;
            else if (m_fGreen < 0) m_fGreen = 0.0;
            
            if (m_fBlue > 1.0) m_fBlue = 1.0;
            else if (m_fBlue < 0) m_fBlue = 0.0;
            
            return *this;
        }
        
        /// wrap color values and keep range [0...1]
        Color &wrap() {
            m_fRed -= floor(m_fRed);
            m_fGreen -= floor(m_fGreen);
            m_fBlue -= floor(m_fBlue);
            return *this;
        }
        
        float      m_fRed;
        float      m_fGreen;
        float      m_fBlue;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_COLOR_H

