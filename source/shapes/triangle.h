#pragma once

#include "core/vec3.h"
#include "shapes/hittable.h"

#include <memory>
#include <string>

class TriangleMesh : public IHittable
{
public:
    TriangleMesh(std::shared_ptr<IMaterial> material);
    ~TriangleMesh();

    bool load(const std::string& objFile);

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hitRecord) const override;
    bool boundingBox(double startTime, double endTime, Aabb& bbox) const override;

private:
    std::shared_ptr<IMaterial> material_;
    Vec3* positions_;
    Vec3* normals_;
    float* uvs_;
    size_t numtris_;
    Aabb bounds_;
};

class Triangle : public IHittable
{
public:
    Triangle(const Vec3& a, const Vec3& b, const Vec3& c, std::shared_ptr<IMaterial> material);

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hitRecord) const override;
    bool boundingBox(double startTime, double endTime, Aabb& bbox) const override;

private:
    std::shared_ptr<IMaterial> material_;
    Vec3 a_;
    Vec3 b_;
    Vec3 c_;
};
