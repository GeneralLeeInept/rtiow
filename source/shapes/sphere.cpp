#include "sphere.h"

#include "core/rtiow.h"

#include <cmath>

bool Sphere::hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const
{
    Vec3 oc = r.origin - center;
    double a = dot(r.direction, r.direction);
    double halfb = dot(oc, r.direction);
    double c = dot(oc, oc) - radius * radius;
    double discriminant = halfb * halfb - a * c;

    if (discriminant < 0)
    {
        return false;
    }

    double sqrtd = std::sqrt(discriminant);
    double root = (-halfb - sqrtd) / a;

    if (root < tMin || root >= tMax)
    {
        root = (-halfb + sqrtd) / a;
    }

    if (root < tMin || root >= tMax)
    {
        return false;
    }

    hit.t = root;
    hit.p = r.at(hit.t);
    Vec3 surfaceNormal = (hit.p - center) / radius;
    hit.setFaceNormal(r, surfaceNormal);
    hit.material = material.get();

    double theta = std::acos(-surfaceNormal.y);
    double phi = std::atan2(-surfaceNormal.z, surfaceNormal.x) + pi;
    hit.u = phi / (2*pi);
    hit.v = theta / pi;

    return true;
}
