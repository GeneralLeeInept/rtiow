#include <cstdint>
#include <iostream>

#include "hittable_list.h"
#include "image.h"
#include "material.h"
#include "ray.h"
#include "rtiow.h"
#include "sphere.h"
#include "vec3.h"

#include "stb_image.h"

#define USESKY 0

struct Sky
{
    int width;
    int height;
    float* data;

    Vec3 Sample(const Vec3& d) const
    {
        double theta, phi;
        d.toSpherical(theta, phi);
        double u = (phi + pi) / (2.0 * pi);
        double v = (1.0 + std::cos(theta)) / 2.0;
        int x = int((width - 1) * u + 0.5);
        int y = int((height - 1) * v + 0.5);
        int i = (x + y * width) * 3;
        return { data[i + 0], data[i + 1], data[i + 2] };
    }
};

Sky loadSky(const std::string_view& hdri)
{
    Sky sky{};
    sky.data = stbi_loadf(hdri.data(), &sky.width, &sky.height, nullptr, 0);
    return sky;
}

Vec3 rayColor(const Ray& r, const HittableList& scene, const Sky& sky, int depth)
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
            return hit.material->Albedo() * rayColor(scattered, scene, sky, depth - 1);
        }
        else
        {
            return Vec3{};
        }
    }
    else
    {
#if USESKY
        return sky.Sample(normalize(r.direction));
#else
        double t = (normalize(r.direction).y + 1.0) * 0.5;
        return lerp(Vec3(1.0, 1.0, 1.0), Vec3(0.5, 0.7, 1.0), t);
#endif
    }
}

int main()
{
    constexpr uint32_t imageWidth = 400;
    constexpr uint32_t imageHeight = 400;
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

    auto greyMatte = std::make_shared<Lambertian>(Vec3(0.5, 0.5, 0.5));
    auto redMatte = std::make_shared<Lambertian>(Vec3(0.9, 0.1, 0.1));
    auto copper = std::make_shared<Metal>(Vec3(0.98, 0.82, 0.75), 0.3);
    auto roughGold = std::make_shared<Metal>(Vec3(1.0, 0.89, 0.61), 0.7);
    auto polishedGold = std::make_shared<Metal>(Vec3(1.0, 0.89, 0.61), 0.1);
    auto goldMatte = std::make_shared<Lambertian>(Vec3(1.0, 0.89, 0.3));
    auto blueGlass = std::make_shared<Dielectric>(Vec3(0.9, 0.9, 1.0), 1.5);
    auto clearGlass = std::make_shared<Dielectric>(1.5);

    HittableList scene{};
    scene.add(std::make_unique<Sphere>(Vec3(-1.1, 0, -1.6), 0.5, copper));
    scene.add(std::make_unique<Sphere>(Vec3(0, 0, -1.6), 0.5, redMatte));
    scene.add(std::make_unique<Sphere>(Vec3(1.1, 0, -1.6), 0.5, blueGlass));
    scene.add(std::make_unique<Sphere>(Vec3(1.1, 0, -1.6), -0.498, blueGlass));
    scene.add(std::make_unique<Sphere>(Vec3(0, -100.5, -1.6), 100, greyMatte));
    Sky sky{};

#if USESKY
    sky = loadSky(R"(R:\assets\hdri\chinese_garden_1k.hdr)");
#endif

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
                color += rayColor(r, scene, sky, maxDepth);
            }

            color *= 1.0 / double(samplesPerPixel);
            Vec3& output = image(x, y);

            for (int c = 0; c < 3; ++c)
            {
                output[c] = clamp(std::pow(color[c], 1.0 / 2.2), 0.0, 1.0);
            }
        }
    }

    image.save("image.png");
    std::cerr << "\nDone.\n";
}
