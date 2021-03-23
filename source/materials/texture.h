#pragma once

#include "core/vec3.h"
#include "core/hit_record.h"

#include <memory>

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
    CheckerTexture(std::shared_ptr<ITexture> even, std::shared_ptr<ITexture> odd)
        : even_(even)
        , odd_(odd)
    {}

    CheckerTexture(const Vec3& evenColor, const Vec3& oddColor)
        : CheckerTexture(std::make_shared<SolidColor>(evenColor), std::make_shared<SolidColor>(oddColor))
    {
    }

    Vec3 sample(const HitRecord& hit) const override
    {
        auto sines = std::sin(10 * hit.p.x) * std::sin(10 * hit.p.y) * std::sin(10 * hit.p.z);
        auto texture = (sines < 0) ? odd_ : even_;
        return texture->sample(hit);
    }

public:
    std::shared_ptr<ITexture> odd_;
    std::shared_ptr<ITexture> even_;
};
