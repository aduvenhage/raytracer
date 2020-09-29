#ifndef LIBS_HEADER_CONSTANTS_H
#define LIBS_HEADER_CONSTANTS_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <random>


constexpr float deg2rad(float _fDeg) {
    return _fDeg / 180.0f * (float)M_PI;
}


constexpr float sqr(float _a) {
    return _a * _a;
}


constexpr bool isPowerOf2(int _n) {
    return (_n > 0) && ((_n & (_n-1)) == 0);
}


template <typename T>
inline auto clamp(const T &_value, const T &_min, const T &_max) {
    auto v = (_value < _min) ? _min : _value;
    return (v > _max) ? _max : v;
}





namespace LNF
{
    const float pi     = M_PI;

    using RandomGen    = std::mt19937_64;
    

};  //namespace LNF


#endif  // #ifndef LIBS_HEADER_CONSTANTS_H
