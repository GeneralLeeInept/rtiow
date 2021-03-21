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
    void setSky(std::shared_ptr<Sky> sky);

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const override;
    std::shared_ptr<const Sky> sky() const;

private:
    std::vector<std::shared_ptr<IHittable>> mObjects;
    std::shared_ptr<Sky> mSky;
};
