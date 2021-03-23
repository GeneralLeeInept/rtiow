#pragma once

#include "core/vec3.h"
#include "materials/texture.h"

#include <memory>

struct HitRecord;
struct Rng;

class Ray;

class IMaterial
{
public:
    virtual ~IMaterial() {}

    virtual bool Scatter(Rng& rng, const Ray& in, const HitRecord& hit, Ray& scattered) const = 0;
    virtual Vec3 Albedo(const HitRecord& hit) const = 0;
    virtual Vec3 Emitted(const HitRecord& hit) const { return Vec3(0, 0, 0); }
};

class Lambertian : public IMaterial
{
public:
    Lambertian() = default;
    Lambertian(const Vec3& color) : albedo_(std::make_shared<SolidColor>(color)) {}
    Lambertian(std::shared_ptr<ITexture> albedo) : albedo_(albedo) {}

    bool Scatter(Rng& rng, const Ray& in, const HitRecord& hit, Ray& scattered) const override;
    Vec3 Albedo(const HitRecord& hit) const override { return albedo_->sample(hit); }

private:
    std::shared_ptr<ITexture> albedo_;
};

class Metal : public IMaterial
{
public:
    Metal() = default;
    Metal(const Vec3& color, double roughness) : albedo_(std::make_shared<SolidColor>(color)), roughness_(roughness) {}
    Metal(std::shared_ptr<ITexture> albedo, double roughness) : albedo_(albedo), roughness_(roughness) {}

    bool Scatter(Rng& rng, const Ray& in, const HitRecord& hit, Ray& scattered) const override;
    Vec3 Albedo(const HitRecord& hit) const override { return albedo_->sample(hit); }

private:
    std::shared_ptr<ITexture> albedo_;
    double roughness_;
};

class Dielectric : public IMaterial
{
public:
    Dielectric() = default;
    Dielectric(std::shared_ptr<ITexture> albedo, double ior) : albedo_(albedo), ior_(ior) {}
    Dielectric(const Vec3& color, double ior) : albedo_(std::make_shared<SolidColor>(color)), ior_(ior) {}
    Dielectric(double ior) : Dielectric(Vec3(1, 1, 1), ior) {}

    bool Scatter(Rng& rng, const Ray& in, const HitRecord& hit, Ray& scattered) const override;
    Vec3 Albedo(const HitRecord& hit) const override { return albedo_->sample(hit); }

private:
    std::shared_ptr<ITexture> albedo_;
    double ior_;
};

class LightSource : public IMaterial
{
public:
    LightSource() = default;
    LightSource(const Vec3& emitted) : emitted_(emitted) {}

    bool Scatter(Rng& rng, const Ray& in, const HitRecord& hit, Ray& scattered) const override { return false; }
    Vec3 Albedo(const HitRecord& hit) const override { return Vec3(0, 0, 0); }
    Vec3 Emitted(const HitRecord& hit) const override;

private:
    Vec3 emitted_;
};
