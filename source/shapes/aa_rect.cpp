#include "aa_rect.h"

#include "core/hit_record.h"

bool RectangleXY::hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const
{
    if (std::abs(r.direction.z) <= 0.0)
    {
        return false;
    }

    double t = (k - r.origin.z) / r.direction.z;

    if (t < tMin || t > tMax)
    {
        return false;
    }

    Vec3 p = r.at(t);

    if (p.x < x0 || p.x > x1 || p.y < y0 || p.y > y1)
    {
        return false;
    }

    hit.t = t;
    hit.setFaceNormal(r, Vec3(0, 0, 1));
    hit.material = material.get();
    hit.p = p;
    hit.u = (p.x - x0) / (x1 - x0);
    hit.v = (p.y - y0) / (y1 - y0);

    return true;
}

bool RectangleXZ::hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const
{
    if (std::abs(r.direction.y) <= 0.0)
    {
        return false;
    }

    double t = (k - r.origin.y) / r.direction.y;

    if (t < tMin || t > tMax)
    {
        return false;
    }

    Vec3 p = r.at(t);

    if (p.x < x0 || p.x > x1 || p.z < z0 || p.z > z1)
    {
        return false;
    }

    hit.t = t;
    hit.setFaceNormal(r, Vec3(0, 1, 0));
    hit.material = material.get();
    hit.p = p;
    hit.u = (p.x - x0) / (x1 - x0);
    hit.v = (p.z - z1) / (z0 - z1);

    return true;
}

bool RectangleYZ::hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const
{
    if (std::abs(r.direction.x) <= 0.0)
    {
        return false;
    }

    double t = (k - r.origin.x) / r.direction.x;

    if (t < tMin || t > tMax)
    {
        return false;
    }

    Vec3 p = r.at(t);

    if (p.y < y0 || p.y > y1 || p.z < z0 || p.z > z1)
    {
        return false;
    }

    hit.t = t;
    hit.setFaceNormal(r, Vec3(1, 0, 0));
    hit.material = material.get();
    hit.p = p;
    hit.u = (p.z - z1) / (z0 - z1);
    hit.v = (p.y - y0) / (y1 - y0);

    return true;
}
