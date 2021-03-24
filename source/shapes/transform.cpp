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

bool Transform::boundingBox(double timeStart, double timeEnd, Aabb& bbox) const
{
    Aabb shapeBbox{};

    if (!shape_->boundingBox(timeStart, timeEnd, shapeBbox))
    {
        return false;
    }

    bbox.mins = Vec3(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    bbox.maxs = Vec3(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest());

    for (int i = 0; i < 8; ++i)
    {
        Vec3 corner = Vec3(transform_ * glm::vec4(shapeBbox.corner(i), 1.0));

        for (int a = 0; a < 3; ++a)
        {
            bbox.mins[a] = std::min(bbox.mins[a], corner[a]);
            bbox.maxs[a] = std::max(bbox.maxs[a], corner[a]);
        }
    }

    return true;
}

bool Transform::boundingSphere(double timeStart, double timeEnd, Vec3& center, double& radius) const
{
    if (!shape_->boundingSphere(timeStart, timeEnd, center, radius))
    {
        return false;
    }

    center = Vec3(transform_ * glm::vec4(center, 1.0));
    return true;
}
