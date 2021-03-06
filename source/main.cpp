#include <cstdint>
#include <iostream>

#include "hittable_list.h"
#include "image.h"
#include "material.h"
#include "ray.h"
#include "rtiow.h"
#include "sphere.h"
#include "vec3.h"

Vec3 rayColor(const Ray& r, const HittableList& scene, int depth)
{
    if (depth == 0)
    {
        return Vec3{};
    }

    HitRecord hit{};
    bool b = scene.Hit(r, 0.001, std::numeric_limits<double>::infinity(), hit);

    if (b)
    {
        Ray scattered;

        if (hit.material->Scatter(r, hit, scattered))
        {
            return hit.material->Albedo() * rayColor(scattered, scene, depth - 1);
        }
        else
        {
            return Vec3{};
        }
    }
    else
    {
        double t = (normalize(r.direction).y + 1.0) * 0.5;
        return lerp(Vec3(1.0, 1.0, 1.0), Vec3(0.5, 0.7, 1.0), t);
    }
}

int main()
{
    constexpr uint32_t imageWidth = 400;
    constexpr uint32_t imageHeight = 300;
    constexpr int samplesPerPixel = 100;
    constexpr double aspectRatio = double(imageWidth) / double(imageHeight);
    constexpr double viewportHeight = 2.0;
    constexpr double viewportWidth = aspectRatio * viewportHeight;
    constexpr double focalLength = 1.0;
    constexpr int maxDepth = 50;

    Image image{ imageWidth, imageHeight };
    Vec3 origin{};
    Vec3 horizontal{ viewportWidth, 0, 0 };
    Vec3 vertical{ 0, viewportHeight, 0 };
    auto viewportOrigin = origin - horizontal / 2.0 - vertical / 2.0 - Vec3(0, 0, focalLength);

    HittableList scene{};
    scene.add(std::make_unique<Sphere>(Vec3(0, 0, -1), 0.5, std::make_shared<Lambertian>(Vec3(1.0, 0.82, 0))));
    scene.add(std::make_unique<Sphere>(Vec3(0,-100.5,-1), 100, std::make_shared<Lambertian>(Vec3(0.5, 0.5, 0.5))));

    for (int y = imageHeight; --y >= 0;)
    {
        std::cerr << "\rScanlines remaining: " << y << ' ' << std::flush;

        for (int x = 0; x < imageWidth; ++x)
        {
            Vec3 color{ 0, 0, 0 };

            for (int s = 0; s < samplesPerPixel; ++s)
            {
                double u = double(x + random()) / (imageWidth - 1);
                double v = double(y + random()) / (imageHeight - 1);
                Ray r(origin, viewportOrigin + horizontal * u + vertical * v - origin);
                color += rayColor(r, scene, maxDepth);
            }

            color *= 1.0 / double(samplesPerPixel);
            Vec3& output = image(x, y);

            for (int c = 0; c < 3; ++c)
            {
                output[c] = std::pow(color[c], 1.0 / 2.2);
            }
        }
    }

    image.save("image.png");
    std::cerr << "\nDone.\n";
}
