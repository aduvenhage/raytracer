#ifndef LIBS_HEADER_CONSTANTS_H
#define LIBS_HEADER_CONSTANTS_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <random>


constexpr double deg2rad(double _dDeg) {
    return _dDeg / 180.0 * M_PI;
}


constexpr double sqr(double _a) {
    return _a * _a;
}


namespace LNF
{
    const double pi     = M_PI;
    const double pi_2   = M_PI/2;
    const double pi_4   = M_PI/4;

    using RandomGen     = std::mt19937_64;

};  //namespace LNF


#endif  // #ifndef LIBS_HEADER_CONSTANTS_H
