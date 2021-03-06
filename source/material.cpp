#include "material.h"

#include "hittable.h"
#include "rtiow.h"

bool Lambertian::Scatter(const Ray& in, const HitRecord& hit, Ray& scattered) const
{
    scattered = { hit.p, hit.n + normalize(randomInUnitSphere()) };

    if (scattered.direction.lengthSq() < std::numeric_limits<double>::epsilon())
    {
        return false;
    }

    return true;
}

bool Metal::Scatter(const Ray& in, const HitRecord& hit, Ray& scattered) const
{
    Vec3 reflected = reflect(normalize(in.direction), hit.n);
    scattered = { hit.p, reflected + randomInUnitSphere() * roughness };
    return (dot(scattered.direction, hit.n) > 0);
}
