#pragma once

#include "hittable.h"
#include "core/vec3.h"
#include "materials/material.h"

#include <memory>

class RectangleXY : public IHittable
{
public:
    double x0, x1, y0, y1, k;
    std::shared_ptr<IMaterial> material;

    RectangleXY() = default;
    RectangleXY(double x0_, double x1_, double y0_, double y1_, double k_, std::shared_ptr<IMaterial> material_)
        : x0(x0_)
        , x1(x1_)
        , y0(y0_)
        , y1(y1_)
        , k(k_)
        , material(material_)
    {
    }

    virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const override;
};

class RectangleXZ : public IHittable
{
public:
    double x0, x1, z0, z1, k;
    std::shared_ptr<IMaterial> material;

    RectangleXZ() = default;
    RectangleXZ(double x0_, double x1_, double z0_, double z1_, double k_, std::shared_ptr<IMaterial> material_)
        : x0(x0_)
        , x1(x1_)
        , z0(z0_)
        , z1(z1_)
        , k(k_)
        , material(material_)
    {
    }

    virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const override;
};

class RectangleYZ : public IHittable
{
public:
    double y0, y1, z0, z1, k;
    std::shared_ptr<IMaterial> material;

    RectangleYZ() = default;
    RectangleYZ(double y0_, double y1_, double z0_, double z1_, double k_, std::shared_ptr<IMaterial> material_)
        : y0(y0_)
        , y1(y1_)
        , z0(z0_)
        , z1(z1_)
        , k(k_)
        , material(material_)
    {
    }

    virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const override;
};

class Box : public IHittable
{
public:
    Vec3 center;
    double radius;
    std::shared_ptr<IMaterial> material;

    Box() = default;
    Box(const Vec3& center_, double radius_, std::shared_ptr<IMaterial> material_) : center(center_), radius(radius_), material(material_) {}

    virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const override;
};
