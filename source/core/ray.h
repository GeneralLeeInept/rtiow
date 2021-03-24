#pragma once

#include "core/vec3.h"

struct Rng;

class Ray
{
public:
    Vec3 origin;
    Vec3 direction;
    double time;
    bool primary;
    Rng* rng;

    Ray() = default;
    Ray(const Ray&) = default;
    Ray(const Vec3& origin_, const Vec3& direction_, double time_, bool primary_, Rng* rng_) : origin(origin_), direction(direction_), time(time_), primary(primary_), rng(rng_) {}

    Ray& operator=(const Ray&) = default;

    Vec3 at(double t) const
    {
        return origin + direction * t;
    }
};
