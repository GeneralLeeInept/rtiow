#pragma once

#include "core/aabb.h"
#include "core/rng.h"
#include "shapes/hittable.h"

#include <memory>
#include <vector>

class HittableList;

class AabbTreeNode : public IHittable
{
public:
    AabbTreeNode() = default;
    AabbTreeNode(const HittableList& list, double timeStart, double timeEnd, Rng& rng);
    AabbTreeNode(const std::vector<std::shared_ptr<IHittable>>& srcobjects, size_t start, size_t end, double timeStart, double timeEnd, Rng& rng);

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hitRecord) const override;
    bool boundingBox(double timeStart, double timeEnd, Aabb& bbox) const override;

private:
    Aabb bounds_;
    std::shared_ptr<IHittable> left_;
    std::shared_ptr<IHittable> right_;
};
