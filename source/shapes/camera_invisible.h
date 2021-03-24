#pragma once
#pragma once

#include "shapes/hittable.h"

#include <memory>

class CameraInvisible : public IHittable
{
public:
    CameraInvisible(std::shared_ptr<IHittable> shape) : shape_(shape) {}

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const override
    {
        if (r.primary)
        {
            return false;
        }

        return shape_->hit(r, tMin, tMax, hit);
    }

    bool boundingBox(double timeStart, double timeEnd, Aabb& bbox) const override
    {
        return shape_->boundingBox(timeStart, timeEnd, bbox);
    }

    bool boundingSphere(double timeStart, double timeEnd, Vec3& center, double& radius) const override
    {
        return shape_->boundingSphere(timeStart, timeEnd, center, radius);
    }

private:
    std::shared_ptr<IHittable> shape_;
};
