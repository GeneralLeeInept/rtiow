#pragma once

#include "hittable.h"

class Sphere : public IHittable
{
public:
    Vec3 center;
    double radius;

    Sphere() = default;
    Sphere(const Vec3& center_, double radius_) : center(center_), radius(radius_) {}

    virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const override;
};
