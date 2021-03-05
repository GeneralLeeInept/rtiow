#pragma once

#include "ray.h"
#include "vec3.h"

struct HitRecord
{
    Vec3 p;
    Vec3 n;
    double t;
};

class IHittable
{
public:
    virtual ~IHittable() {};

    virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const = 0;
};
