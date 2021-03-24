#pragma once

#include "core/vec3.h"

class Ray;

struct Aabb
{
    Aabb() = default;
    Aabb(const Vec3& mins, const Vec3& maxs);

    bool hit(const Ray& r, double tMin, double tMax) const;

    Aabb makeUnion(const Aabb& other) const;
    Vec3 extents() const;
    Vec3 corner(int index) const;

    Vec3 mins;
    Vec3 maxs;
};
