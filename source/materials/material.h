#pragma once

#include "core/vec3.h"

struct HitRecord;
struct Rng;

class Ray;

class IMaterial
{
public:
    virtual ~IMaterial() {}

    virtual bool Scatter(Rng& rng, const Ray& in, const HitRecord& hit, Ray& scattered) const = 0;
    virtual Vec3 Albedo() const = 0;
    virtual Vec3 Emitted(const HitRecord& hit) const { return Vec3(0,0,0); }
};

class Lambertian : public IMaterial
{
public:
    Lambertian() = default;
    Lambertian(const Vec3& albedo_) : albedo(albedo_) {}

    bool Scatter(Rng& rng, const Ray& in, const HitRecord& hit, Ray& scattered) const override;
    Vec3 Albedo() const override { return albedo; }

    Vec3 albedo;
};

class Metal : public IMaterial
{
public:
    Metal() = default;
    Metal(const Vec3& albedo_, double roughness_) : albedo(albedo_), roughness(roughness_) {}

    bool Scatter(Rng& rng, const Ray& in, const HitRecord& hit, Ray& scattered) const override;
    Vec3 Albedo() const override { return albedo; }

    Vec3 albedo;
    double roughness;
};

class Dielectric : public IMaterial
{
public:
    Dielectric() = default;
    Dielectric(double ior_) : albedo(1, 1, 1), ior(ior_) {}
    Dielectric(const Vec3& albedo_, double ior_) : albedo(albedo_), ior(ior_) {}

    bool Scatter(Rng& rng, const Ray& in, const HitRecord& hit, Ray& scattered) const override;
    Vec3 Albedo() const override { return albedo; }

    Vec3 albedo;
    double ior;
};

class LightSource : public IMaterial
{
public:
    LightSource() = default;
    LightSource(const Vec3& emitted_) : emitted(emitted_) {}

    bool Scatter(Rng& rng, const Ray& in, const HitRecord& hit, Ray& scattered) const override { return false; }
    Vec3 Albedo() const override { return Vec3(0, 0, 0); }
    Vec3 Emitted(const HitRecord& hit) const override;

    Vec3 emitted;
};