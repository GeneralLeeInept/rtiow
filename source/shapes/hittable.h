#pragma once

#include "core/ray.h"
#include "core/vec3.h"

class IMaterial;
struct HitRecord;

class IHittable
{
public:
    virtual ~IHittable() {};

    virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const = 0;
};
