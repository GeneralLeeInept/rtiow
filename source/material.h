#pragma once

#include "vec3.h"

struct HitRecord;

class Ray;

class IMaterial
{
public:
    virtual ~IMaterial() {}

    virtual bool Scatter(const Ray& in, const HitRecord& hit, Ray& scattered) const = 0;
    virtual Vec3 Albedo() const = 0;
};

class Lambertian : public IMaterial
{
public:
    Lambertian() = default;
    Lambertian(const Vec3& albedo_) : albedo(albedo_) {}

    bool Scatter(const Ray& in, const HitRecord& hit, Ray& scattered) const override;
    Vec3 Albedo() const override { return albedo; }

    Vec3 albedo;
};

class Metal : public IMaterial
{
public:
    Metal() = default;
    Metal(const Vec3& albedo_, double roughness_) : albedo(albedo_), roughness(roughness_) {}

    bool Scatter(const Ray& in, const HitRecord& hit, Ray& scattered) const override;
    Vec3 Albedo() const override { return albedo; }

    Vec3 albedo;
    double roughness;
};
