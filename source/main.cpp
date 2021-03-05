#include "stb_image_write.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <string_view>

struct Image
{
    uint32_t width;
    uint32_t height;
    std::unique_ptr<uint8_t[]> bits;
};

std::unique_ptr<Image> createImage(uint32_t width, uint32_t height)
{
    std::unique_ptr<Image> image = std::make_unique<Image>();
    image->width = width;
    image->height = height;
    image->bits = std::make_unique<uint8_t[]>(width * height * 3);
    return image;
}

bool saveImage(Image& image, const std::string_view& path)
{
    stbi_flip_vertically_on_write(1);
    return !!stbi_write_png(path.data(), image.width, image.height, 3, image.bits.get(), image.width * 3);
}

int main()
{
    auto image = createImage(640, 480);

    for (int j = 640-1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;

        for (int i = 0; i < 480; ++i)
        {
            auto r = double(i) / (480-1);
            auto g = double(j) / (640-1);
            auto b = 0.25;

            uint8_t ir = static_cast<uint8_t>(255.999 * r);
            uint8_t ig = static_cast<uint8_t>(255.999 * g);
            uint8_t ib = static_cast<uint8_t>(255.999 * b);

            image->bits[(i * 640 * 3 + j * 3) + 0] = ir;
            image->bits[(i * 640 * 3 + j * 3) + 1] = ig;
            image->bits[(i * 640 * 3 + j * 3) + 2] = ib;
        }
    }

    saveImage(*image, "image.png");
    std::cerr << "\nDone.\n";
}
