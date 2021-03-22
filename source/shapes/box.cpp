#include "box.h"

#include <cmath>

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

    return true;
}

bool Box::hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const
{
    double tx, ty, tz;
    Vec3 nx, ny, nz;

    // hits on +x/-x
    if (std::abs(r.direction.x) > 0.0)
    {
        double invdx = 1.0 / r.direction.x;
        double t1 = (-radius - r.origin.x) * invdx;
        double t2 = (radius - r.origin.x) * invdx;
        double sn = 1.0;

        if (t2 < t1)
        {
            std::swap(t1, t2);
            sn = -1.0;
        }

        if (t1 > tMin)
        {
            tx = t1;
            nx = Vec3(sn, 0, 0);
        }
        else
        {
            tx = t2;
            nx = Vec3(-sn, 0, 0);
        }

        tMin = std::max(tMin, t1);
        tMax = std::min(tMax, t2);
    }
    else if (std::abs(r.origin.x) >= radius)
    {
        return false;
    }

    // hits on +y/-y
    if (std::abs(r.direction.y) > 0.0)
    {
        double invdy = 1.0 / r.direction.y;
        double t1 = (-radius - r.origin.y) * invdy;
        double t2 = (radius - r.origin.y) * invdy;
        double sn = 1.0;

        if (t2 < t1)
        {
            std::swap(t1, t2);
            sn = -1.0;
        }

        if (t1 > tMin)
        {
            ty = t1;
            ny = Vec3(0, sn, 0);
        }
        else
        {
            ty = t2;
            ny = Vec3(0, -sn, 0);
        }

        tMin = std::max(tMin, t1);
        tMax = std::min(tMax, t2);
    }
    else if (std::abs(r.origin.y) >= radius)
    {
        return false;
    }

    // hits on +z/-z
    if (std::abs(r.direction.z) > 0.0)
    {
        double invdz = 1.0 / r.direction.z;
        double t1 = (-radius - r.origin.z) * invdz;
        double t2 = (radius - r.origin.z) * invdz;
        double sn = 1.0;

        if (t2 < t1)
        {
            std::swap(t1, t2);
            sn = -1.0;
        }

        if (t1 > tMin)
        {
            tz = t1;
            nz = Vec3(0, 0, sn);
        }
        else
        {
            tz = t2;
            nz = Vec3(0, 0, -sn);
        }

        tMin = std::max(tMin, t1);
        tMax = std::min(tMax, t2);
    }
    else if (std::abs(r.origin.z) >= radius)
    {
        return false;
    }

    if (tMin >= tMax)
    {
        return false;
    }

    if (tMin == tx)
    {
        hit.t = tx;
        hit.n = nx;
    }
    else if (tMin == ty)
    {
        hit.t = ty;
        hit.n = ny;
    }
    else if (tMin == tz)
    {
        hit.t = tz;
        hit.n = nz;
    }
    else if (tMax == tx)
    {
        hit.t = tx;
        hit.n = nx;
    }
    else if (tMax == ty)
    {
        hit.t = ty;
        hit.n = ny;
    }
    else if (tMax == tz)
    {
        hit.t = tz;
        hit.n = nz;
    }

    hit.p = r.at(hit.t);
    hit.material = material.get();

    return true;
}
