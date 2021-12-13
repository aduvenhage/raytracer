#ifndef CORE_CONSTANTS_H
#define CORE_CONSTANTS_H

#define _USE_MATH_DEFINES
#include <cmath>


#define ALIGN __attribute__((aligned(64)))


constexpr float pif = (float)M_PI;


// NOTE: this will not work values > -32768
constexpr float myfloorf(float _a) {
	return (int)(_a + 32768.0f) - 32768.0f;
}


constexpr float mytruncf(float _a) {
    return (float)((int)_a);
}


constexpr float deg2rad(float _fDeg) {
    return _fDeg / 180.0f * pif;
}


constexpr float angleWrap2Pi(float _a) {
    return _a - pif*2 * myfloorf(_a/(pif*2));
}


constexpr float sqr(float _a) {
    return _a * _a;
}


constexpr bool isPowerOf2(int _n) {
    return (_n > 0) && ((_n & (_n-1)) == 0);
}


template <typename T>
constexpr auto clamp(const T &_value, const T &_min, const T &_max) {
    auto v = (_value < _min) ? _min : _value;
    return (v > _max) ? _max : v;
}


constexpr float fracf(float _a) {
    return _a - mytruncf(_a);
}


constexpr float minf(float _a, float _b) {
	return _a < _b ? _a : _b;
}


constexpr float maxf(float _a, float _b) {
	return _a > _b ? _a : _b;
}


constexpr int signf(float _value)
{
	return (_value > 0) - (_value < 0);
}


constexpr float absf(float _value)
{
    return _value * signf(_value);
}

#endif  // #ifndef CORE_CONSTANTS_H
