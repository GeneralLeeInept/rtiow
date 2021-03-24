#include "aabb.h"

#include "ray.h"

#include <algorithm>

Aabb::Aabb(const Vec3& mins_, const Vec3& maxs_)
    : mins(mins_)
    , maxs(maxs_)
{
}

bool Aabb::hit(const Ray& r, double tMin, double tMax) const
{
    for (int a = 0; a < 3; a++)
    {
        auto invD = 1.0f / r.direction[a];
        auto t0 = (mins[a] - r.origin[a]) * invD;
        auto t1 = (maxs[a] - r.origin[a]) * invD;

        if (invD < 0.0f)
        {
            std::swap(t0, t1);
        }

        tMin = t0 > tMin ? t0 : tMin;
        tMax = t1 < tMax ? t1 : tMax;

        if (tMax <= tMin)
        {
            return false;
        }
    }

    return true;
}

Aabb Aabb::makeUnion(const Aabb& other) const
{
    Aabb result{};

    for (int a = 0; a < 3; ++a)
    {
        result.mins[a] = std::min(mins[a], other.mins[a]);
        result.maxs[a] = std::max(maxs[a], other.maxs[a]);
    }

    return result;
}

Vec3 Aabb::extents() const
{
    return maxs - mins;
}

Vec3 Aabb::corner(int index) const
{
    int x = index & 1;
    int y = (index & 2) >> 1;
    int z = (index & 4) >> 2;
    return Vec3(x ? maxs.x : mins.x, y ? maxs.y : mins.y, z ? maxs.z : mins.z);
}
