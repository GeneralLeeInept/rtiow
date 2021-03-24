#include "constant_medium.h"

#include "core/rng.h"

ConstantMedium::ConstantMedium(std::shared_ptr<IHittable> boundary, double density, std::shared_ptr<ITexture> albedo)
    : boundary_(boundary)
    , negInvDensity_(-1/density)
    , phaseFunction_(std::make_shared<Isotropic>(albedo))
{
}

ConstantMedium::ConstantMedium(std::shared_ptr<IHittable> boundary, double density, const Vec3& color)
    : boundary_(boundary)
    , negInvDensity_(-1/density)
    , phaseFunction_(std::make_shared<Isotropic>(color))
{
}

bool ConstantMedium::hit(const Ray& r, double tMin, double tMax, HitRecord& hitRecord) const
{
    HitRecord rec1, rec2;

    if (!boundary_->hit(r, -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), rec1))
    {
        return false;
    }

    if (!boundary_->hit(r, rec1.t + 0.0001, std::numeric_limits<double>::infinity(), rec2))
    {
        return false;
    }

    rec1.t = std::max(rec1.t, tMin);
    rec2.t = std::min(rec2.t, tMax);

    if (rec1.t >= rec2.t)
    {
        return false;
    }

    if (rec1.t < 0)
    {
        rec1.t = 0;
    }

    double rayLength = length(r.direction);
    double distanceTravelledThroughMedium = (rec2.t - rec1.t) * rayLength;
    double hitDistance = negInvDensity_ * std::log(r.rng->operator()());
    
    if (hitDistance > distanceTravelledThroughMedium)
    {
        return false;
    }

    hitRecord.t = rec1.t + hitDistance / rayLength;
    hitRecord.p = r.at(hitRecord.t);
    hitRecord.n = Vec3(0,0,0);  // no normal
    hitRecord.frontFace = true; // arbitrary
    hitRecord.material = phaseFunction_.get();

    return true;
}

bool ConstantMedium::boundingBox(double startTime, double endTime, Aabb& bbox) const
{
    return boundary_->boundingBox(startTime, endTime, bbox);
}
