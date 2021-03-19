#pragma once

#include "core/ray.h"
#include "core/vec3.h"

class IMaterial;

struct HitRecord
{
    Vec3 p;
    Vec3 n;
    double t;
    bool frontFace;
    IMaterial* material;

    void setFaceNormal(const Ray& r, const Vec3& surfaceNormal)
    {
        frontFace = dot(r.direction, surfaceNormal) < 0;
        n = frontFace ? surfaceNormal :-surfaceNormal;
    }
};

class IHittable
{
public:
    virtual ~IHittable() {};

    virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const = 0;
};
