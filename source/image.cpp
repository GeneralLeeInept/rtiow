#include "image.h"

#include <cassert>
#include <vector>

#include "stb_image_write.h"

Image::Image(uint32_t width, uint32_t height)
{
    mWidth = width;
    mHeight = height;
    mPixels = std::make_unique<Vec3[]>(width * height);
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
                ptr[i] = static_cast<uint8_t>(255.0 * p.v[i] + 0.5);
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
