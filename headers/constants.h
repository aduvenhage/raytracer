#ifndef LIBS_HEADER_CONSTANTS_H
#define LIBS_HEADER_CONSTANTS_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <random>


constexpr float deg2rad(float _dDeg) {
    return _dDeg / 180.0f * M_PI;
}


constexpr float sqr(float _a) {
    return _a * _a;
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
