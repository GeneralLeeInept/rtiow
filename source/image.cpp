#include "image.h"

#include <cassert>
#include <vector>

#include "rtiow.h"
#include "stb_image_write.h"

Image::Image(uint32_t width, uint32_t height)
{
    mWidth = width;
    mHeight = height;
    mPixels.resize(width * height);
    memset(&mPixels[0], 0, width * height * sizeof(Vec3));
}

uint32_t Image::width() const
{
    return mWidth;
}

uint32_t Image::height() const
{
    return mHeight;
}

Vec3& Image::operator()(uint32_t x, uint32_t y)
{
    assert(x < mWidth && y < mHeight);
    return mPixels[x + y * mWidth];
}

Vec3 Image::operator()(uint32_t x, uint32_t y) const
{
    assert(x < mWidth && y < mHeight);
    return mPixels[x + y * mWidth];
}

bool Image::save(const std::string_view& path) const
{
    std::vector<uint8_t> bits(mWidth * mHeight * 3);
    uint8_t* ptr = bits.data();

    for (uint32_t y = 0; y < mHeight; ++y)
    {
        for (uint32_t x = 0; x < mWidth; ++x)
        {
            Vec3 p = operator()(x, y);

            for (int i = 0; i < 3; ++i)
            {
                double e = clamp(std::pow(p[i], 1.0 / 2.2), 0.0, 1.0);
                ptr[i] = static_cast<uint8_t>(255.0 * e + 0.5);
            }

            ptr += 3;
        }
    }

    stbi_flip_vertically_on_write(1);
    return !!stbi_write_png(path.data(), mWidth, mHeight, 3, bits.data(), mWidth * 3);
}

bool Image::saveHDR(const std::string_view& path) const
{
    std::vector<float> data(mWidth * mHeight * 3);
    float* ptr = &data[0];

    for (uint32_t y = 0; y < mHeight; ++y)
    {
        for (uint32_t x = 0; x < mWidth; ++x)
        {
            Vec3 p = operator()(x, y);

            for (int i = 0; i < 3; ++i)
            {
                ptr[i] = float(p.v[i]);
            }

            ptr += 3;
        }
    }

    stbi_flip_vertically_on_write(1);
    return !!stbi_write_hdr(path.data(), mWidth, mHeight, 3, &data[0]);
}

Image& Image::operator+=(const Image& rhs)
{
    size_t len = mPixels.size();
    Vec3* dst = &mPixels[0];
    const Vec3* src = &rhs.mPixels[0];

    for (size_t i = 0; i < len; ++i)
    {
        dst[i] += src[i];
    }

    return *this;
}

Image& Image::operator/=(double s)
{
    size_t len = mPixels.size();
    double scale = 1.0 / s;
    Vec3* dst = &mPixels[0];

    for (size_t i = 0; i < len; ++i)
    {
        dst[i] *= scale;
    }

    return *this;
}
