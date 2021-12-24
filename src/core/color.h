#pragma once

#include "constants.h"
#include "stats.h"


namespace CORE
{
    /*
       R-G-B color class.
       Color components are floats [0..1].
     */
    struct Color
    {
        enum class OPERATION {
            CLAMP = 1,
            WRAP = 2
        };

        Color() noexcept = default;
        Color(float _fRed, float _fGreen, float _fBlue) noexcept
            :m_c{_fRed, _fGreen, _fBlue}
        {}
        
        Color(float _fRed, float _fGreen, float _fBlue, const OPERATION &_op) noexcept
            :m_c{ _fRed, _fGreen, _fBlue}
        {
            if (_op == OPERATION::CLAMP) clamp();
            else if (_op == OPERATION::WRAP) wrap();
        }

        float red() const {return m_c[0];}
        float &red() {return m_c[0];}
        float green() const {return m_c[1];}
        float &green() {return m_c[1];}
        float blue() const {return m_c[2];}
        float &blue() {return m_c[2];}

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
            m_c[0] -= floorf(m_c[0]);
            m_c[1] -= floorf(m_c[1]);
            m_c[2] -= floorf(m_c[2]);
            return *this;
        }
        
        bool isBlack() {
            return m_c[0] + m_c[1] + m_c[2] < 0.0001f;
        }
        
        float max() const {
            return std::max(m_c[0], std::max(m_c[1], m_c[2]));
        }
        
        Color &gammaCorrect2() {
            m_c[0] = sqrt(m_c[0]);
            m_c[1] = sqrt(m_c[1]);
            m_c[2] = sqrt(m_c[2]);
            return *this;
        }
        
        float      m_c[3] = {};
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
        static const Color  White(1, 1, 1);
        static const Color  Black(0, 0, 0);
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
            return Color((float)m_red.mean(),
                         (float)m_green.mean(),
                         (float)m_blue.mean());
        }

     private:
        RunningStat     m_red;
        RunningStat     m_green;
        RunningStat     m_blue;
    };



};  // namespace CORE

