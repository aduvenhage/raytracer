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
            :m_c{}
        {}
        
        Color(float _fRed, float _fGreen, float _fBlue)
            :m_c{_fRed, _fGreen, _fBlue}
        {}
        
        Color(const Color &) = default;
        Color(Color &&) = default;
        Color(Color &) = default;
        
        float red() const {return m_c[0];}
        float &red() {return m_c[0];}
        float green() const {return m_c[1];}
        float &green() {return m_c[1];}
        float blue() const {return m_c[2];}
        float &blue() {return m_c[2];}

        Color &operator=(const Color &) = default;
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
            return m_c[0] + m_c[1] + m_c[2] < 0.00001f;
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
    
    
    //
    class RunningStat
    {
     public:
        RunningStat()
            :m_n(0),
             m_oldM(0),
             m_newM(0),
             m_oldS(0),
             m_newS(0),
             m_maturity(0)
        {}

        void clear(){
            m_n = 0;
            m_newM = 0;
            m_oldS = 0;
            m_newS = 0;
            m_maturity = 0;
        }

        void push(double x) {
            m_n++;
            
            if (m_n == 1) {
                m_newM = x;
                m_newS = 0.0;
            }
            else {
                m_newM = m_oldM + (x - m_oldM) / m_n;
                m_newS = m_oldS + (x - m_oldM) * (x - m_newM);
            }
            
            m_maturity = fabs(m_newM - m_oldM);
            m_oldM = m_newM;
            m_oldS = m_newS;
        }

        int numDataValues() const {
            return m_n;
        }

        double mean() const {
            return m_newM;
        }

        double variance() const {
            return ((m_n > 1) ? m_newS / (m_n - 1) : 0.0);
        }

        double standardDeviation() const {
            return sqrt(variance());
        }
        
        double maturity() const {
            return m_maturity;
        }

     private:
        int         m_n;
        double      m_oldM;
        double      m_newM;
        double      m_oldS;
        double      m_newS;
        double      m_maturity;
    };

    
    //
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
        
        double maturity() const {
            return m_red.maturity() +
                   m_green.maturity() +
                   m_blue.maturity();
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

