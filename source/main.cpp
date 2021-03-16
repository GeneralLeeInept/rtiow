#include <cstdint>
#include <iostream>

#include "camera.h"
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
        double v = dot(d, { 0, 1, 0 });
        Vec3 projxz = d;
        projxz.y = 0;
        double u = dot(normalize(projxz), { 1, 0, 0 }) * 0.5;

        if (d.z >= 0.0)
        {
            u = (u + 1) * 0.5;
        }
        else
        {
            u = (-u + 1) * 0.5;
        }

        v = (v + 1) * 0.5;

        int x = int((width - 1) * u + 0.5);
        int y = int((height - 1) * (1 - v) + 0.5);
        //x = clamp(x, 0, width - 1);
        //y = clamp(y, 0, height - 1);
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

static HittableList randomScene();

int main()
{
    constexpr uint32_t imageWidth = 1600;
    constexpr uint32_t imageHeight = 900;
    constexpr int samplesPerPixel = 500;
    constexpr double aspectRatio = double(imageWidth) / double(imageHeight);
    constexpr int maxDepth = 50;

    Image image{ imageWidth, imageHeight };
    Vec3 cameraPos(13,2,3);
    Vec3 cameraTarget(0,0,0);
    double focalLength = 10.0;
    double aperature = 0.1;
    Camera camera(cameraPos, cameraTarget, Vec3(0, 1, 0), degToRad(30), aspectRatio, aperature, focalLength);

    HittableList scene = randomScene();

    Sky sky{};

#if USESKY
    //sky = loadSky(R"(R:\assets\hdri\kloppenheim_02_4k.hdr)");
    sky = loadSky(R"(R:\assets\hdri\chinese_garden_4k.hdr)");
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
                Ray r = camera.createRay(u, v);

                Vec3 contribution = {};
                for (bool valid = false; !valid;)
                {
                    contribution = rayColor(r, scene, sky, maxDepth);
                    valid = true;

                    for (int i = 0; valid && i < 3; ++i)
                    {
                        valid = valid && !std::isnan(contribution[i]);
                        valid = valid && !std::isinf(contribution[i]);
                    }
                }

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

    image.saveHDR("image.hdr");
    image.save("image.png");
    std::cerr << "\nDone.\n";
}

HittableList randomScene()
{
    HittableList world;

    auto ground_material = std::make_shared<Lambertian>(Vec3(0.5, 0.5, 0.5));
    world.add(std::make_unique<Sphere>(Vec3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto chooseMat = random();
            Vec3 center(a + 0.9 * random(), 0.2, b + 0.9 * random());

            if ((center - Vec3(4, 0.2, 0)).length() > 0.9)
            {
                std::shared_ptr<IMaterial> sphereMaterial;

                if (chooseMat < 0.8)
                {
                    // diffuse
                    auto albedo = randomColor() * randomColor();
                    sphereMaterial = std::make_shared<Lambertian>(albedo);
                    world.add(std::make_unique<Sphere>(center, 0.2, sphereMaterial));
                }
                else if (chooseMat < 0.95)
                {
                    // metal
                    auto albedo = randomColor(0.5, 1);
                    auto fuzz = random(0, 0.5);
                    sphereMaterial = std::make_shared<Metal>(albedo, fuzz);
                    world.add(std::make_unique<Sphere>(center, 0.2, sphereMaterial));
                }
                else if (chooseMat < 0.975)
                {
                    // glass
                    sphereMaterial = std::make_shared<Dielectric>(1.5);
                    world.add(std::make_unique<Sphere>(center, 0.2, sphereMaterial));
                }
                else
                {
                    // glass bubble
                    sphereMaterial = std::make_shared<Dielectric>(1.5);
                    world.add(std::make_unique<Sphere>(center, 0.2, sphereMaterial));
                    world.add(std::make_unique<Sphere>(center, -0.18, sphereMaterial));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_unique<Sphere>(Vec3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Vec3(0.4, 0.2, 0.1));
    world.add(std::make_unique<Sphere>(Vec3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Vec3(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_unique<Sphere>(Vec3(4, 1, 0), 1.0, material3));

    return world;
}