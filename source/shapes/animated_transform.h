#pragma once

#include "core/quat.h"
#include "shapes/hittable.h"

#include <memory>
#include <vector>

class AnimatedTransform : public IHittable
{
public:
    struct KeyFrame
    {
        Quat rotation;
        Vec3 position;
        double time;
    };

    AnimatedTransform(std::shared_ptr<IHittable> shape) : shape_(shape) {}

    void addKeyFrame(const KeyFrame& keyframe);
    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const override;

private:
    std::vector<KeyFrame> keyframes_;
    std::shared_ptr<IHittable> shape_;
};
