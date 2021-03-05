#include "stb_image_write.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <string_view>
#include <vector>

#include "ray.h"
#include "vec3.h"

template<typename T>
T lerp(const T& from, const T& to, double t)
{
    return from * (1.0 - t) + to * t;
}

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

bool hitSphere(const Vec3& center, double radius, const Ray& r)
{
    Vec3 oc = r.origin - center;
    double a = dot(r.direction, r.direction);
    double b = 2.0 * dot(oc, r.direction);
    double c = dot(oc, oc) - radius * radius;
    double discriminant = b * b - 4.0 * a * c;
    return (discriminant > 0.0);
}

Vec3 rayColor(const Ray& r)
{
    double t = 0.5 * (r.direction.y + 1.0);

    if (hitSphere(Vec3(0.0, 0.0, -1.0), 0.5, r))
    {
        return Vec3(1, 0, 0);
    }

    return lerp(Vec3(1.0, 1.0, 1.0), Vec3(0.5, 0.7, 1.0), t);
}

int main()
{
    constexpr uint32_t imageWidth = 1920;
    constexpr uint32_t imageHeight = 1080;
    auto image = createImage(imageWidth, imageHeight);
    double aspectRatio = double(imageWidth) / double(imageHeight);

    double viewportHeight = 2.0;
    double viewportWidth = aspectRatio * viewportHeight;
    double focal_length = 1.0;

    Vec3 origin{};
    Vec3 horizontal{ viewportWidth, 0, 0 };
    Vec3 vertical{ 0, viewportHeight, 0 };
    auto viewportOrigin = origin - horizontal / 2.0 - vertical / 2.0 - Vec3(0, 0, focal_length);

    for (int j = imageHeight - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;

        for (int i = 0; i < imageWidth; ++i)
        {
            double u = double(i) / (imageWidth - 1);
            double v = double(j) / (imageHeight - 1);
            Ray r(origin, viewportOrigin + horizontal * u + vertical * v - origin);
            image->pixels[i + j * imageWidth] = rayColor(r);
        }
    }

    saveImage(*image, "image.png");
    std::cerr << "\nDone.\n";
}
