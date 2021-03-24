#include "texture.h"

#include "core/rtiow.h"
#include "stb_image.h"

CheckerTexture::CheckerTexture(std::shared_ptr<ITexture> even, std::shared_ptr<ITexture> odd)
    : even_(even)
    , odd_(odd)
{
}

CheckerTexture::CheckerTexture(const Vec3& evenColor, const Vec3& oddColor)
    : even_(std::make_shared<SolidColor>(evenColor))
    , odd_(std::make_shared<SolidColor>(oddColor))
{
}

Vec3 CheckerTexture::sample(const HitRecord& hit) const
{
    // FIXME - this doesn't work for x/y/z constant zero
    auto sines = std::sin(10 * hit.p.x) * std::sin(10 * hit.p.y) * std::sin(10 * hit.p.z);
    auto texture = (sines < 0) ? odd_ : even_;
    return texture->sample(hit);
}

ImageTexture::ImageTexture(std::string_view path)
{
    if (!load(path))
    {
        exit(EXIT_FAILURE);
    }
}

bool ImageTexture::load(std::string_view path)
{
    stbi_uc* pixels = stbi_load(path.data(), &width_, &height_, nullptr, 3);

    if (!pixels)
    {
        return false;
    }

    int size = width_ * height_;
    data_.resize(size);

    stbi_uc* src = pixels;
    double c = 1.0 / 255.0;

    for (int i = 0; i < size; ++i, src += 3)
    {
        data_[i] = Vec3(src[0] * c, src[1] * c, src[2] * c);
    }

    stbi_image_free(pixels);
    return true;
}

Vec3 ImageTexture::sample(const HitRecord& hit) const
{
    double u = clamp(hit.u, 0.0, 1.0);
    double v = 1.0 - clamp(hit.v, 0.0, 1.0);
    int x = int(u * (width_ - 1));
    int y = int(v * (height_ - 1));
    return data_[x + y * width_];
}

NoiseTexture::NoiseTexture(double scale)
    : scale_(scale)
{
}

Vec3 NoiseTexture::sample(const HitRecord& hit) const
{
    return Vec3(1, 1, 1) * 0.5 * (1 + std::sin(scale_ * hit.p.z + 10 * noise_.turb(hit.p)));
}
