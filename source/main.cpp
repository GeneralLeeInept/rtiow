#include <cstdint>
#include <iostream>
#include <random>

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

template<typename T>
T clamp(const T& value, const T& min, const T& max)
{
    return (value < min) ? min : ((value > max) ? max : value);
}

inline double random()
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random(double min, double max)
{
    return lerp(min, max, random());
}

Ray scatterRay(const Ray& r, const HitRecord& hit)
{
    Vec3 randvec;
    
    for (;;)
    {
        randvec = { random(-1, 1), random(-1, 1), random(-1, 1) };
        if (randvec.lengthSq() < 1) break;
    }

    Ray scattered = { hit.p, hit.n + normalize(randvec) };
    return scattered;
}

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
        return rayColor(scatterRay(r, hit), scene, depth - 1) * 0.5;
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
    constexpr uint32_t imageHeight = 225;
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
    scene.add(std::make_unique<Sphere>(Vec3(0, 0, -1), 0.5));
    scene.add(std::make_unique<Sphere>(Vec3(0,-100.5,-1), 100));

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
