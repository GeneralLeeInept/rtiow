#pragma once

#include "core/vec3.h"

class Ray
{
public:
    Vec3 origin;
    Vec3 direction;
    double time;
    bool primary;

    Ray() = default;
    Ray(const Ray&) = default;
    Ray(const Vec3& origin_, const Vec3& direction_, double time_, bool primary_) : origin(origin_), direction(direction_), time(time_), primary(primary_) {}

    Ray& operator=(const Ray&) = default;

    Vec3 at(double t) const
    {
        return origin + direction * t;
    }
};
