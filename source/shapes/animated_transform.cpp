#include "animated_transform.h"

#include "core/mat4.h"
#include "core/rtiow.h"

void AnimatedTransform::addKeyFrame(const KeyFrame& keyframe)
{
    auto insertPos = std::begin(keyframes_);
    auto sentinel = std::end(keyframes_);

    for (; insertPos != sentinel && insertPos->time < keyframe.time; insertPos = std::next(insertPos));
    keyframes_.insert(insertPos, keyframe);
}

bool AnimatedTransform::hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const
{
    if (keyframes_.empty())
    {
        return shape_->hit(r, tMin, tMax, hit);
    }

    auto endFrame = std::begin(keyframes_);
    for ( ; endFrame->time < r.time && std::next(endFrame) != std::end(keyframes_); endFrame = std::next(endFrame));

    auto startFrame = (endFrame == std::begin(keyframes_)) ? endFrame : std::prev(endFrame);

    double t = (endFrame->time == startFrame->time) ? 0 : (r.time - startFrame->time) / (endFrame->time - startFrame->time);
    t =  clamp(t , 0.0, 1.0);

    Quat q = glm::slerp(startFrame->rotation, endFrame->rotation, t);
    Vec3 p = lerp(startFrame->position, endFrame->position, t);

    Mat4 transform = glm::translate(p) * glm::mat4_cast(q);
    Mat4 invTransform = inverse(transform);

    Ray rt{};
    rt.origin = Vec3(invTransform * glm::vec4(r.origin, 1.0));
    rt.direction = Vec3(invTransform * glm::vec4(r.direction, 0.0));

    if (!shape_->hit(rt, tMin, tMax, hit))
    {
        return false;
    }

    hit.p = Vec3(transform * glm::vec4(hit.p, 1.0));
    hit.n = Vec3(transform * glm::vec4(hit.n, 0.0));
    return true;
}
