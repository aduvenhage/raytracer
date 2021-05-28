#ifndef LIBS_HEADER_COLOR_H
#define LIBS_HEADER_COLOR_H

#include "constants.h"
#include "stats.h"


namespace LNF
{
    /*
       R-G-B color class.
       Color components are floats [0..1].
     */
    struct Color
    {
        Color() noexcept
            :m_c{}
        {}
        
        Color(float _fRed, float _fGreen, float _fBlue) noexcept
            :m_c{_fRed, _fGreen, _fBlue}
        {}
        
        Color(const Color &) noexcept = default;
        Color(Color &&) noexcept = default;
        Color(Color &) noexcept = default;
        
        float red() const {return m_c[0];}
        float &red() {return m_c[0];}
        float green() const {return m_c[1];}
        float &green() {return m_c[1];}
        float blue() const {return m_c[2];}
        float &blue() {return m_c[2];}

        Color &operator=(const Color &) noexcept = default;
        Color &operator=(Color &&) = default;
        
        Color operator+(const Color &_color) const {
            return Color(m_c[0] + _color.m_c[0],
                         m_c[1] + _color.m_c[1],
                         m_c[2] + _color.m_c[2]);
        }
        
        Color operator-(const Color &_color) const {
            return Color(m_c[0] - _color.m_c[0],
                         m_c[1] - _color.m_c[1],
                         m_c[2] - _color.m_c[2]);
        }
        
        Color &operator+=(const Color &_color) {
            m_c[0] += _color.m_c[0];
            m_c[1] += _color.m_c[1];
            m_c[2] += _color.m_c[2];
            return *this;
        }
        
        Color &operator-=(const Color &_color) {
            m_c[0] -= _color.m_c[0];
            m_c[1] -= _color.m_c[1];
            m_c[2] -= _color.m_c[2];
            return *this;
        }
        
        Color operator*(float _fScale) const {
            return Color(m_c[0] * _fScale,
                         m_c[1] * _fScale,
                         m_c[2] * _fScale);
        }
        
        Color &operator*=(float _fScale) {
            m_c[0] *= _fScale;
            m_c[1] *= _fScale;
            m_c[2] *= _fScale;
            return *this;
        }
        
        Color operator/(float _fScale) const {
            return Color(m_c[0] / _fScale,
                         m_c[1] / _fScale,
                         m_c[2] / _fScale);
        }
        
        Color &operator/=(float _fScale) {
            m_c[0] /= _fScale;
            m_c[1] /= _fScale;
            m_c[2] /= _fScale;
            return *this;
        }
        
        // 3 element scale
        Color operator*(const Color &_color) const {
            return Color(m_c[0] * _color.m_c[0],
                         m_c[1] * _color.m_c[1],
                         m_c[2] * _color.m_c[2]);
        }
        
        // 3 element scale
        Color &operator*=(const Color &_color) {
            m_c[0] *= _color.m_c[0];
            m_c[1] *= _color.m_c[1];
            m_c[2] *= _color.m_c[2];
            return *this;
        }
        
        // set color values to 0 if smaller and 1 if larger.
        Color &clamp() {
            m_c[0] = ::clamp(m_c[0], 0.0f, 1.0f);
            m_c[1] = ::clamp(m_c[1], 0.0f, 1.0f);
            m_c[2] = ::clamp(m_c[2], 0.0f, 1.0f);
            
            return *this;
        }
        
        // wrap color values and keep range [0...1]
        Color &wrap() {
            m_c[0] -= floor(m_c[0]);
            m_c[1] -= floor(m_c[1]);
            m_c[2] -= floor(m_c[2]);
            return *this;
        }
        
        bool isBlack() {
            return m_c[0] + m_c[1] + m_c[2] < 0.0001f;
        }
        
        float      m_c[3];
    };
    
    
    // scale color
    Color operator*(float _fScale, const Color &_color) {
        return _color * _fScale;
    }
    
    
    // preset colors
    namespace COLOR
    {
        static const Color  Red(1, 0, 0);
        static const Color  Green(0, 1, 0);
        static const Color  Blue(0, 0, 1);
    }
    
    
    // Running stats combined for color
    class ColorStat {
     public:
        ColorStat()
        {}
        
        void push(const Color &_color) {
            m_red.push(_color.red());
            m_green.push(_color.green());
            m_blue.push(_color.blue());
        }
        
        double variance() const {
            return m_red.variance() +
                   m_green.variance() +
                   m_blue.variance();
        }
        
        double standardDeviation() const {
            return m_red.standardDeviation() +
                   m_green.standardDeviation() +
                   m_blue.standardDeviation();
        }
        
        Color mean() const {
            return Color(m_red.mean(),
                         m_green.mean(),
                         m_blue.mean());
        }

     private:
        RunningStat     m_red;
        RunningStat     m_green;
        RunningStat     m_blue;
    };



};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_COLOR_H

