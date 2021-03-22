#pragma once

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

private:
    std::shared_ptr<IHittable> shape_;
};
