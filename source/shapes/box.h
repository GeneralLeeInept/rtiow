#pragma once

#include "core/vec3.h"
#include "shapes/hittable_list.h"

#include <memory>
#include <vector>

class Box : public IHittable
{
public:
    Box() = default;
    Box(const Vec3& extents, std::shared_ptr<IMaterial> material);

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const override;

private:
    HittableList sides_;
};
