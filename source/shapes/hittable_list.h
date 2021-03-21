#pragma once

#include "hittable.h"

#include <memory>
#include <vector>

class HittableList : public IHittable
{
public:
    HittableList() = default;

    void clear();
    void add(std::shared_ptr<IHittable> object);

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const override;

private:
    std::vector<std::shared_ptr<IHittable>> mObjects;
};
