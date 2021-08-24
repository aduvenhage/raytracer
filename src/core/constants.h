#ifndef CORE_CONSTANTS_H
#define CORE_CONSTANTS_H

#define _USE_MATH_DEFINES
#include <cmath>


#define ALIGN __attribute__((aligned(64)))


constexpr float pi    = (float)M_PI;
constexpr float pi2    = (float)(M_PI*2);


constexpr float deg2rad(float _fDeg) {
    return _fDeg / 180.0f * pi;
}


inline float angleWrap2Pi(float _a) {
    return _a - pi2 * floor(_a/pi2);
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


constexpr uint32_t roundUpPow2(uint32_t _v) {
    _v--;
    _v |= _v >> 1;
    _v |= _v >> 2;
    _v |= _v >> 4;
    _v |= _v >> 8;
    _v |= _v >> 16;
    _v++;
    
    return _v;
}


constexpr uint64_t roundUpPow2(uint64_t _v) {
    _v--;
    _v |= _v >> 1;
    _v |= _v >> 2;
    _v |= _v >> 4;
    _v |= _v >> 8;
    _v |= _v >> 16;
    _v |= _v >> 32;
    _v++;
    
    return _v;
}



#endif  // #ifndef CORE_CONSTANTS_H
