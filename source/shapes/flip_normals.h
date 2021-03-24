#pragma once

#include "core/hit_record.h"
#include "shapes/hittable.h"

#include <memory>

class FlipNormals : public IHittable
{
public:
    FlipNormals(std::shared_ptr<IHittable> shape) : shape_(shape) {}
    
    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const override
    {
        if (!shape_->hit(r, tMin, tMax, hit))
        {
            return false;
        }

        hit.frontFace = !hit.frontFace;
        return true;
    }

    bool boundingBox(double startTime, double endTime, Aabb& bbox) const override
    {
        return shape_->boundingBox(startTime, endTime, bbox);
    }

    bool boundingSphere(double startTime, double endTime, Vec3& center, double& radius) const override
    {
        return shape_->boundingSphere(startTime, endTime, center, radius);
    }

private:
    std::shared_ptr<IHittable> shape_;
};
