#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

#include "vec3.h"

class Image
{
public:
    Image() = default;
    Image(uint32_t width, uint32_t height);
    Image(const Image&) = delete;
    Image(Image&&) = default;

    Image& operator=(const Image&) = delete;
    Image& operator=(Image&&) = default;

    uint32_t width() const;
    uint32_t height() const;

    Vec3& operator()(uint32_t x, uint32_t y);
    Vec3 operator()(uint32_t x, uint32_t y) const;

    bool save(const std::string_view& path) const;

private:
    uint32_t mWidth;
    uint32_t mHeight;
    std::unique_ptr<Vec3[]> mPixels;
};
