#pragma once

#include "core/ray.h"
#include "core/vec3.h"

struct Rng;

class Camera
{
public:
    struct CreateInfo
    {
        Vec3 position;
        Vec3 target;
        Vec3 vup;
        double fovy;
        double aperature;
        double focalDistance;
        double timeBegin;
        double timeEnd;
    };

    Camera(const CreateInfo& createInfo, double aspectRatio);

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
    double timeBegin_;
    double timeEnd_;
};
