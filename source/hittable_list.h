#pragma once

#include "hittable.h"

#include <memory>
#include <vector>

class HittableList : public IHittable
{
public:
    HittableList() = default;

    void clear();
    void add(std::unique_ptr<IHittable> object);

    bool Hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const override;

private:
    std::vector<std::unique_ptr<IHittable>> mObjects;
};
