#pragma once

#include "core/aabb.h"
#include "core/ray.h"
#include "core/vec3.h"

class IMaterial;
struct HitRecord;

class IHittable
{
public:
    virtual ~IHittable() {};

    virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& hitRecord) const = 0;
    virtual bool boundingBox(double startTime, double endTime, Aabb& bbox) const = 0;

    virtual bool boundingSphere(double startTime, double endTime, Vec3& center, double& radius) const
    {
        Aabb bbox;

        if (!boundingBox(startTime, endTime, bbox))
        {
            return false;
        }

        center = (bbox.mins + bbox.maxs) / 2.0;
        radius = length(bbox.extents() / 2.0);
        return true;
    }
};
