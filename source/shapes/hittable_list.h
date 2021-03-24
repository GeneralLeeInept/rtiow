#pragma once

#include "hittable.h"
#include "core/sky.h"

#include <memory>
#include <vector>

class HittableList : public IHittable
{
public:
    HittableList() = default;

    void clear();
    void add(std::shared_ptr<IHittable> object);

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const override;
    bool boundingBox(double startTime, double endTime, Aabb& bbox) const override;

    const std::vector<std::shared_ptr<IHittable>>& objects() const { return objects_; }
    std::shared_ptr<const IHittable> operator[](size_t index) const { return objects_[index]; }

private:
    std::vector<std::shared_ptr<IHittable>> objects_;
};
