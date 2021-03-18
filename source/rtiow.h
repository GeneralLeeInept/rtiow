#pragma once

#include "vec3.h"

template<typename T>
T lerp(const T& from, const T& to, double t)
{
    return from * (1.0 - t) + to * t;
}

template<typename T>
T clamp(const T& value, const T& min, const T& max)
{
    return (value < min) ? min : ((value > max) ? max : value);
}

constexpr double pi = 3.1415926535897932384626433832795;

constexpr double degToRad(double deg)
{
    return deg * pi / 180.0;
}
