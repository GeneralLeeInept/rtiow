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

inline Vec3 randomColor()
{
    return Vec3{ random(), random(), random() };
}

inline Vec3 randomColor(double min, double max)
{
    return Vec3{ random(min, max), random(min, max), random(min, max) };
}

inline Vec3 randomInUnitSphere()
{
    for (;;)
    {
        Vec3 v = { random(-1, 1), random(-1, 1), random(-1, 1) };

        if (v.lengthSq() < 1)
        {
            return v;
        }
    }
}

inline Vec3 randomInUnitDisk()
{
    for (;;)
    {
        Vec3 v = Vec3(random(-1,1), random(-1,1), 0);

        if (v.lengthSq() < 1)
        {
            return v;
        }
    }
}

constexpr double pi = 3.1415926535897932384626433832795;

constexpr double degToRad(double deg)
{
    return deg * pi / 180.0;
}
