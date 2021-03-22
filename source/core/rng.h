#pragma once

#include <random>

#include "rtiow.h"

struct Rng
{
    Rng()
        : distribution_(0, 1)
    {
        std::random_device rd;
        std::seed_seq ss({ rd(), rd(), rd(), rd(), rd(), rd() });
        generator_.seed(ss);
    }

    Rng(unsigned int seed)
        : Rng()
    {
        generator_.seed(seed);
    }

    double operator()()
    {
        return distribution_(generator_);
    }

    double operator()(double min, double max)
{
    return lerp(min, max, operator()());
}

    Vec3 color()
    {
        return Vec3{ operator()(), operator()(), operator()() };
    }

    Vec3 color(double min, double max)
    {
        return Vec3{ operator()(min, max), operator()(min, max), operator()(min, max) };
    }

    Vec3 inUnitSphere()
    {
        for (;;)
        {
            Vec3 v = { operator()(-1, 1), operator()(-1, 1), operator()(-1, 1) };

            if (length2(v) < 1)
            {
                return v;
            }
        }
    }

    Vec3 inUnitDisk()
    {
        for (;;)
        {
            Vec3 v = Vec3(operator()(-1,1), operator()(-1,1), 0);

            if (length(v) < 1)
            {
                return v;
            }
        }
    }

    std::uniform_real_distribution<double> distribution_;
    std::mt19937 generator_;
};
