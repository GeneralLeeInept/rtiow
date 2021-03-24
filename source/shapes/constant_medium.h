#pragma once

#include "shapes/hittable.h"
#include "materials/material.h"
#include "materials/texture.h"

#include <memory>

class ConstantMedium : public IHittable
{
public:
    ConstantMedium(std::shared_ptr<IHittable> boundary, double density, std::shared_ptr<ITexture> albedo);
    ConstantMedium(std::shared_ptr<IHittable> boundary, double density, const Vec3& color);

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hitRecord) const override;
    bool boundingBox(double startTime, double endTime, Aabb& bbox) const override;

public:
    std::shared_ptr<IHittable> boundary_;
    std::shared_ptr<IMaterial> phaseFunction_;
    double negInvDensity_;
};
