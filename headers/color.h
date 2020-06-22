#ifndef LIBS_HEADER_COLOR_H
#define LIBS_HEADER_COLOR_H

#include "constants.h"


namespace LNF
{
    /**
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
        
        Color operator*(double _dScale) const {
            return Color(m_fRed*_dScale,
                         m_fGreen*_dScale,
                         m_fBlue*_dScale);
        }
        
        float      m_fRed;
        float      m_fGreen;
        float      m_fBlue;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_COLOR_H

