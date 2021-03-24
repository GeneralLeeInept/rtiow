#pragma once

#include "sky.h"

#include "core/rtiow.h"

#include <cmath>

#include "stb_image.h"

HdriSky::~HdriSky()
{
    if (data_)
    {
        stbi_image_free(data_);
    }
}

bool HdriSky::load(std::string_view path)
{
    data_ = stbi_loadf(path.data(), &width_, &height_, nullptr, 0);
    return !!data_;
}

Vec3 HdriSky::Sample(const Vec3& d) const
{
    double theta = std::acos(clamp(d.y, -1.0, 1.0));
    double phi = std::atan2(d.z, d.x);
    phi = (phi < 0) ? (phi + 2.0 * pi) : phi;
    double u = phi / (2.0 * pi);
    double v = theta / pi;
    int x = int((width_ - 1) * u + 0.5);
    int y = int((height_ - 1) * v + 0.5);
    int i = (x + y * width_) * 3;
    return { data_[i + 0], data_[i + 1], data_[i + 2] };
}

GradientSky::GradientSky(Vec3 nadirColor, Vec3 zenithColor)
    : nadirColor_(nadirColor)
    , zenithColor_(zenithColor)
{
}

Vec3 GradientSky::Sample(const Vec3& d) const
{
    double t = (d.y + 1.0) * 0.5;
    return lerp(nadirColor_, zenithColor_, t);
}

ConstantColorSky::ConstantColorSky(Vec3 color)
    : color_(color)
{
}

Vec3 ConstantColorSky::Sample(const Vec3&) const
{
    return color_;
}
