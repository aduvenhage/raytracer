#ifndef CORE_CONSTANTS_H
#define CORE_CONSTANTS_H

#define _USE_MATH_DEFINES
#include <cmath>


#define ALIGN __attribute__((aligned(64)))


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


inline float frac(float _a) {
    return _a - std::trunc(_a);
}


namespace CORE
{
    const float pi     = (float)M_PI;

};  // namespace CORE


#endif  // #ifndef CORE_CONSTANTS_H
