#include "stb_image_write.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <string_view>
#include <vector>

#include "vec3.h"

struct Image
{
    uint32_t width;
    uint32_t height;
    std::unique_ptr<Vec3[]> pixels;
};

std::unique_ptr<Image> createImage(uint32_t width, uint32_t height)
{
    std::unique_ptr<Image> image = std::make_unique<Image>();
    image->width = width;
    image->height = height;
    image->pixels = std::make_unique<Vec3[]>(width * height);
    return image;
}

bool saveImage(Image& image, const std::string_view& path)
{
    stbi_flip_vertically_on_write(1);

    uint32_t pixelCount = image.width * image.height;
    std::vector<uint8_t> bits(pixelCount * 3);
    uint8_t* ptr = bits.data();

    for (uint32_t p = 0; p < pixelCount; ++p)
    {
        for (int i = 0; i < 3; ++i)
        {
            *ptr++ = static_cast<uint8_t>(255.999 * image.pixels[p].v[i]);
        }
    }

    return !!stbi_write_png(path.data(), image.width, image.height, 3, bits.data(), image.width * 3);
}

int main()
{
    constexpr uint32_t imageWidth = 640;
    constexpr uint32_t imageHeight = 640;
    auto image = createImage(imageWidth, imageHeight);

    for (int j = imageHeight - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;

        for (int i = 0; i < imageWidth; ++i)
        {
            auto r = double(i) / (imageWidth-1);
            auto g = double(j) / (imageHeight-1);
            auto b = 0.25;
            image->pixels[i + j * imageWidth] = Vec3(r, g, b);
        }
    }

    saveImage(*image, "image.png");
    std::cerr << "\nDone.\n";
}
