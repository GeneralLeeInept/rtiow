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

Vec3 refract(const Vec3& uv, const Vec3& n, double etaiOverEtat)
{
    double cos_theta = std::min(dot(-uv, n), 1.0);
    Vec3 rOutPerp =  (uv + n * cos_theta) * etaiOverEtat;
    Vec3 rOutParallel = n * -std::sqrt(std::abs(1.0 - rOutPerp.lengthSq()));
    return rOutPerp + rOutParallel;
}

double reflectance(double cosine, double refractionRatio)
{
    // Use Schlick's approximation for reflectance.
    double r0 = (1 - refractionRatio) / (1 + refractionRatio);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}

bool Dielectric::Scatter(const Ray& in, const HitRecord& hit, Ray& scattered) const
{
    double refractionRatio = hit.frontFace ? (1.0 / ior) : ior;
    Vec3 unitDirection = normalize(in.direction);
    double cosTheta = std::min(dot(-unitDirection, hit.n), 1.0);
    double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

    bool cannotRefract = refractionRatio * sinTheta > 1.0;

    if (cannotRefract || reflectance(cosTheta, refractionRatio) > random())
    {
        Vec3 reflected = reflect(unitDirection, hit.n);
        scattered = { hit.p, reflected };
    }
    else
    {
        Vec3 refracted = refract(unitDirection, hit.n, refractionRatio);
        scattered = { hit.p, refracted };
    }

    return true;
}
