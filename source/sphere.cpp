#include "sphere.h"

#include <cmath>

bool Sphere::Hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const
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
    hit.n = (hit.p - center) / radius;
    hit.material = material.get();

    return true;
}
