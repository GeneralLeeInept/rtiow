#pragma once

#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

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

    bool save(std::string_view path) const;
    bool saveHDR(std::string_view path) const;

    Image& operator+=(const Image& rhs);
    Image& operator/=(double s);

private:
    uint32_t mWidth;
    uint32_t mHeight;
    std::vector<Vec3> mPixels;
};
