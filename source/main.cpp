#include <cstdint>
#include <iostream>
#include <optional>

#include "hittable_list.h"
#include "image.h"
#include "ray.h"
#include "sphere.h"
#include "vec3.h"

template<typename T>
T lerp(const T& from, const T& to, double t)
{
    return from * (1.0 - t) + to * t;
}

Vec3 rayColor(const Ray& r, const HittableList& scene)
{
    HitRecord hit{};
    bool b = scene.Hit(r, 0, std::numeric_limits<double>::infinity(), hit);

    if (b)
    {
        return (hit.n + Vec3(1, 1, 1)) * 0.5;
    }
    else
    {
        double t = 0.5 * (r.direction.y + 1.0);
        return lerp(Vec3(1.0, 1.0, 1.0), Vec3(0.5, 0.7, 1.0), t);
    }
}

int main()
{
    constexpr uint32_t imageWidth = 1920;
    constexpr uint32_t imageHeight = 1080;
    constexpr double aspectRatio = double(imageWidth) / double(imageHeight);
    constexpr double viewportHeight = 2.0;
    constexpr double viewportWidth = aspectRatio * viewportHeight;
    constexpr double focal_length = 1.0;

    Image image{ imageWidth, imageHeight };
    Vec3 origin{};
    Vec3 horizontal{ viewportWidth, 0, 0 };
    Vec3 vertical{ 0, viewportHeight, 0 };
    auto viewportOrigin = origin - horizontal / 2.0 - vertical / 2.0 - Vec3(0, 0, focal_length);

    HittableList scene{};
    scene.add(std::make_unique<Sphere>(Vec3(0, 0, -1), 0.5));
    scene.add(std::make_unique<Sphere>(Vec3(0,-100.5,-1), 100));
    for (int y = imageHeight; --y >= 0;)
    {
        std::cerr << "\rScanlines remaining: " << y << ' ' << std::flush;

        for (int x = 0; x < imageWidth; ++x)
        {
            double u = double(x) / (imageWidth - 1);
            double v = double(y) / (imageHeight - 1);
            Ray r(origin, viewportOrigin + horizontal * u + vertical * v - origin);
            image(x, y) = rayColor(r, scene);
        }
    }

    image.save("image.png");
    std::cerr << "\nDone.\n";
}
