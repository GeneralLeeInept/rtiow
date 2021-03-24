#pragma once

#include "core/ray.h"
#include "core/vec3.h"

class IMaterial;

struct HitRecord
{
    Vec3 p;
    Vec3 n;
    double t;
    double u;
    double v;
    bool frontFace;
    IMaterial* material;

    void setFaceNormal(const Ray& r, const Vec3& surfaceNormal)
    {
        frontFace = dot(r.direction, surfaceNormal) < 0;
        n = frontFace ? surfaceNormal :-surfaceNormal;
    }
};
