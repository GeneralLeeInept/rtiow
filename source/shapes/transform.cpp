#include "transform.h"

#include "core/hit_record.h"

bool Transform::hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const
{
    Ray rt;
    rt.origin = Vec3(invTransform_ * glm::vec4(r.origin, 1.0));
    rt.direction = Vec3(invTransform_ * glm::vec4(r.direction, 0.0));

    if (!shape_->hit(rt, tMin, tMax, hit))
    {
        return false;
    }

    hit.p = Vec3(transform_ * glm::vec4(hit.p, 1.0));
    hit.n = Vec3(transform_ * glm::vec4(hit.n, 0.0));
    return true;
}
