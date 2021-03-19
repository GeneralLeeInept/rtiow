#pragma once

#include "hittable.h"
#include "core/vec3.h"
#include "materials/material.h"

#include <memory>

class Sphere : public IHittable
{
public:
    Vec3 center;
    double radius;
    std::shared_ptr<IMaterial> material;

    Sphere() = default;
    Sphere(const Vec3& center_, double radius_, std::shared_ptr<IMaterial> material_) : center(center_), radius(radius_), material(material_) {}

    virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const override;
};
