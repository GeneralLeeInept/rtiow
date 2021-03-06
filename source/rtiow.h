#pragma once

#include <random>

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

inline double random()
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random(double min, double max)
{
    return lerp(min, max, random());
}

inline Vec3 randomVec(double min, double max)
{
    return Vec3{ random(-1, 1), random(-1, 1), random(-1, 1) };
}

inline Vec3 randomInUnitSphere()
{
    Vec3 result;

    for (;;)
    {
        result = { random(-1, 1), random(-1, 1), random(-1, 1) };
        if (result.lengthSq() < 1) break;
    }

    return result;
}

constexpr double pi = 3.1415926535897932384626433832795;
