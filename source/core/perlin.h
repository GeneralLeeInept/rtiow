#pragma once

#include "core/vec3.h"

struct Rng;

class Perlin
{
public:
    Perlin();
    ~Perlin();

    double noise(const Vec3& p) const;
    double turb(const Vec3& p, int depth = 7) const;

private:
    static const int pointCount = 256;
    Vec3* ranvec_;
    int* xperm_;
    int* yperm_;
    int* zperm_;
};
