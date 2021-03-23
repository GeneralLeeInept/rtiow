#pragma once

#include "core/vec3.h"

#include <string_view>

class Sky
{
public:
    virtual ~Sky() = default;

    virtual Vec3 Sample(const Vec3& d) const = 0;
};

class HdriSky : public Sky
{
public:
    ~HdriSky();

    bool load(std::string_view path);
    Vec3 Sample(const Vec3& d) const override;

private:
    int width_;
    int height_;
    float* data_;
};

class GradientSky : public Sky
{
public:
    GradientSky(Vec3 nadirColor, Vec3 zenithColor);

    Vec3 Sample(const Vec3& d) const override;

private:
    Vec3 nadirColor_;
    Vec3 zenithColor_;
};

class ConstantColorSky : public Sky
{
public:
    ConstantColorSky(Vec3 color);

    Vec3 Sample(const Vec3& d) const override;

private:
    Vec3 color_;
};
