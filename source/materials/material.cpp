#include "material.h"

#include "core/rng.h"
#include "core/rtiow.h"
#include "shapes/hittable.h"

bool Lambertian::Scatter(Rng& rng, const Ray& in, const HitRecord& hit, Ray& scattered) const
{
    Vec3 scatterDirection = hit.n + rng.inUnitSphere();

    if (length2(scatterDirection) < std::numeric_limits<double>::epsilon())
    {
        return false;
    }

    scattered = { hit.p, normalize(scatterDirection), in.time, false };
    return true;
}

bool Metal::Scatter(Rng& rng, const Ray& in, const HitRecord& hit, Ray& scattered) const
{
    Vec3 reflected = reflect(in.direction, hit.n);
    Vec3 scatterDirection = reflected + rng.inUnitSphere() * roughness_;

    if (dot(scatterDirection, hit.n) <= 0)
    {
        return false;
    }

    scattered = { hit.p, normalize(scatterDirection), in.time, false };
    return true;
}

Vec3 refract(const Vec3& uv, const Vec3& n, double etaiOverEtat)
{
    double cos_theta = std::min(dot(-uv, n), 1.0);
    Vec3 rOutPerp =  (uv + n * cos_theta) * etaiOverEtat;
    Vec3 rOutParallel = n * -std::sqrt(std::abs(1.0 - length2(rOutPerp)));
    return rOutPerp + rOutParallel;
}

double reflectance(double cosine, double refractionRatio)
{
    // Use Schlick's approximation for reflectance.
    double r0 = (1 - refractionRatio) / (1 + refractionRatio);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}

bool Dielectric::Scatter(Rng& rng, const Ray& in, const HitRecord& hit, Ray& scattered) const
{
    double refractionRatio = hit.frontFace ? (1.0 / ior_) : ior_;
    double cosTheta = std::min(dot(-in.direction, hit.n), 1.0);
    double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

    bool cannotRefract = refractionRatio * sinTheta > 1.0;

    if (cannotRefract || reflectance(cosTheta, refractionRatio) > rng())
    {
        Vec3 reflected = normalize(reflect(in.direction, hit.n));
        scattered = { hit.p, reflected, in.time, false };
    }
    else
    {
        Vec3 refracted = normalize(refract(in.direction, hit.n, refractionRatio));
        scattered = { hit.p, refracted, in.time, false };
    }

    return true;
}

Vec3 LightSource::Emitted(const HitRecord& hit) const 
{
    return hit.frontFace ? emitted_ : Albedo(hit);
}
