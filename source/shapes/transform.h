#pragma once

#include "core/mat4.h"
#include "shapes/hittable.h"

#include <memory>

class Transform : public IHittable
{
public:
    Transform(const Mat4& transform, std::shared_ptr<IHittable> shape)
        : transform_(transform)
        , shape_(shape)
    {
        invTransform_ = glm::inverse(transform);
    }

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const override;

private:
    Mat4 transform_;
    Mat4 invTransform_;
    std::shared_ptr<IHittable> shape_;
};