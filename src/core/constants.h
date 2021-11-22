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
    return _a - pi2 * floorf(_a/pi2);
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


inline float minf(float _a, float _b) {
	return _a < _b ? _a : _b;
}


inline float maxf(float _a, float _b) {
	return _a > _b ? _a : _b;
}


// NOTE: this will not work values > -32768
inline float myfloorf(float _a) {
	return (int)(_a + 32768.0f) - 32768.0f;
}





#endif  // #ifndef CORE_CONSTANTS_H
