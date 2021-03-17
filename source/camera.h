#pragma once

#include "ray.h"
#include "vec3.h"

struct Rng;

class Camera
{
public:
    Camera(const Vec3& position, const Vec3& target, const Vec3& up, double fovy, double aspectRatio, double aperature, double focalDistance);

    Ray createRay(Rng& rng, double s, double t) const;

private:
    Vec3 position_;
    Vec3 lowerLeftCorner_;
    Vec3 horizontal_;
    Vec3 vertical_;
    Vec3 u_;
    Vec3 v_;
    Vec3 w_;
    double lensRadius_;
};
