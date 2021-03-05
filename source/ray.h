#pragma once

#include "vec3.h"

class Ray
{
public:
    Vec3 origin;
    Vec3 direction;

    Ray() = default;
    Ray(const Ray&) = default;
    Ray(const Vec3& origin_, const Vec3& direction_) : origin(origin_), direction(direction_) {}

    Ray& operator=(const Ray&) = default;

    Vec3 at(double t) const
    {
        return origin + direction * t;
    }
};
