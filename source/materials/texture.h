#pragma once

#include "core/vec3.h"
#include "core/hit_record.h"

#include <memory>
#include <string_view>
#include <vector>

class ITexture
{
public:
    virtual ~ITexture() = default;

    virtual Vec3 sample(const HitRecord& hit) const = 0;
};

class SolidColor : public ITexture
{
public:
    SolidColor(const Vec3& color) : color_(color) {}

    Vec3 sample(const HitRecord& hit) const override { return color_; }

private:
    Vec3 color_;
};

class CheckerTexture : public ITexture
{
public:
    CheckerTexture(std::shared_ptr<ITexture> even, std::shared_ptr<ITexture> odd);
    CheckerTexture(const Vec3& evenColor, const Vec3& oddColor);

    Vec3 sample(const HitRecord& hit) const override;

private:
    std::shared_ptr<ITexture> odd_;
    std::shared_ptr<ITexture> even_;
};

class ImageTexture : public ITexture
{
public:
    ImageTexture() = default;
    ImageTexture(std::string_view path);

    bool load(std::string_view path);

    Vec3 sample(const HitRecord& hit) const override;

private:
    int width_;
    int height_;
    std::vector<Vec3> data_;
};
