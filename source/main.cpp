#include <iostream>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <thread>

#include "camera.h"
#include "command_line.h"
#include "hittable_list.h"
#include "image.h"
#include "material.h"
#include "ray.h"
#include "rng.h"
#include "rtiow.h"
#include "sphere.h"
#include "vec3.h"

#include "stb_image.h"

#define USESKY 0
#define FILTER_NANS 0

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

Vec3 rayColor(const Ray& r, const HittableList& scene, const Sky& sky, Rng& rng, int depth)
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

        if (hit.material->Scatter(rng, r, hit, scattered))
        {
            return hit.material->Albedo() * rayColor(scattered, scene, sky, rng, depth - 1);
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

static HittableList randomScene(Rng& rng);

struct Pass
{
    Pass(uint32_t width, uint32_t height)
        : image(width, height)
    {
    }

    void render(const HittableList& scene, const Camera& camera, const Sky& sky, int maxDepth)
    {
        for (int y = int(image.height()); --y >= 0;)
        {
            for (int x = 0; x < int(image.width()); ++x)
            {
                double u = double(x + rng()) / (image.width() - 1);
                double v = double(y + rng()) / (image.height() - 1);
                Ray r = camera.createRay(rng, u, v);

#if FILTER_NANS
                Vec3 contribution = {};
                for (bool valid = false; !valid;)
                {
                    contribution = rayColor(r, scene, sky, rng, maxDepth);
                    valid = true;

                    for (int i = 0; valid && i < 3; ++i)
                    {
                        valid = valid && !std::isnan(contribution[i]);
                        valid = valid && !std::isinf(contribution[i]);
                    }
                }
                image(x, y) = contribution;
#else
                image(x, y) = rayColor(r, scene, sky, rng, maxDepth);
#endif
            }
        }
    }

    Image image;
    Rng rng;
};

struct Job
{
    Job(uint32_t width, uint32_t height)
        : image(width, height)
    {
    }

    struct Args
    {
        const HittableList* scene;
        const Camera* camera;
        const Sky* sky;
        int numPasses;
        int maxDepth;
    };

    void run(const HittableList& scene, const Camera& camera, const Sky& sky, int numPasses, int maxDepth)
    {
        thread_ = std::thread(&Job::threadFunc, this, scene, camera, sky, numPasses, maxDepth);
    }

    void wait()
    {
        thread_.join();
    }

    void threadFunc(const HittableList& scene, const Camera& camera, const Sky& sky, int numPasses, int maxDepth)
    {
        for (int y = int(image.height()); --y >= 0;)
        {
            for (int x = 0; x < int(image.width()); ++x)
            {
                Vec3 color{};

                for (int s = 0; s < numPasses; ++s)
                {
                    double u = double(x + rng_()) / (image.width() - 1);
                    double v = double(y + rng_()) / (image.height() - 1);
                    Ray r = camera.createRay(rng_, u, v);

#if FILTER_NANS
                    Vec3 contribution = {};
                    for (bool valid = false; !valid;)
                    {
                        contribution = rayColor(r, scene, sky, rng, maxDepth);
                        valid = true;

                        for (int i = 0; valid && i < 3; ++i)
                        {
                            valid = valid && !std::isnan(contribution[i]);
                            valid = valid && !std::isinf(contribution[i]);
                        }
                    }
                    color += contribution;
#else
                    color += rayColor(r, scene, sky, rng_, maxDepth);
#endif
                }

                image(x, y) = color;
            }
        }
    }

    Image image;
    Rng rng_;
    std::thread thread_;
};

int main(int argc, char** argv)
{
    CommandLineArguments args{};
    args.imageWidth = 512;
    args.imageHeight = 512;
    args.samplesPerPixel = 100;
    args.maxDepth = 50;
    args.numJobs = 4;
    args.outputName = "image";

    if (!parseCommandLine(argc, argv, args))
    {
        exit(EXIT_FAILURE);
    }

    double aspectRatio = double(args.imageWidth) / double(args.imageHeight);

    Image image{ args.imageWidth, args.imageHeight };
    Vec3 cameraPos(13, 2, 3);
    Vec3 cameraTarget(0, 0, 0);
    double focalLength = 10.0;
    double aperature = 0.1;
    Camera camera(cameraPos, cameraTarget, Vec3(0, 1, 0), degToRad(30), aspectRatio, aperature, focalLength);

    Rng rng{};
    HittableList scene = randomScene(rng);

    Sky sky{};

#if USESKY
    //sky = loadSky(R"(R:\assets\hdri\kloppenheim_02_4k.hdr)");
    sky = loadSky(R"(R:\assets\hdri\chinese_garden_4k.hdr)");
#endif

    uint32_t passesPerJob = args.samplesPerPixel / args.numJobs;
    uint32_t extraPasses = args.samplesPerPixel % args.numJobs;
    std::vector<Job> jobs;

    for (uint32_t i = 0; i < args.numJobs; ++i)
    {
        jobs.push_back(Job(args.imageWidth, args.imageHeight));
    }

    std::cerr << "Running " << args.numJobs << " jobs...\n";
    auto startTime = std::chrono::system_clock::now();

    for (uint32_t i = 0; i < extraPasses; ++i)
    {
        jobs[i].run(scene, camera, sky, passesPerJob + 1, args.maxDepth);
    }

    for (uint32_t i = extraPasses; i < args.numJobs; ++i)
    {
        jobs[i].run(scene, camera, sky, passesPerJob, args.maxDepth);
    }

    for (Job& j : jobs)
    {
        j.wait();
        image += j.image;
    }

    image /= args.samplesPerPixel;

    auto endTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();

    image.saveHDR(args.outputName + ".hdr");
    image.save(args.outputName + ".png");
    std::cerr << "\nDone. " << duration << " seconds.\n";
}

HittableList randomScene(Rng& rng)
{
    HittableList world;

    auto ground_material = std::make_shared<Lambertian>(Vec3(0.5, 0.5, 0.5));
    world.add(std::make_shared<Sphere>(Vec3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            double chooseMat = rng();
            Vec3 center(a + 0.9 * rng(), 0.2, b + 0.9 * rng());

            if ((center - Vec3(4, 0.2, 0)).length() > 0.9)
            {
                std::shared_ptr<IMaterial> sphereMaterial;

                if (chooseMat < 0.8)
                {
                    // diffuse
                    Vec3 albedo = rng.color() * rng.color();
                    sphereMaterial = std::make_shared<Lambertian>(albedo);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
                else if (chooseMat < 0.95)
                {
                    // metal
                    Vec3 albedo = rng.color(0.5, 1);
                    double fuzz = rng(0, 0.5);
                    sphereMaterial = std::make_shared<Metal>(albedo, fuzz);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
                else if (chooseMat < 0.975)
                {
                    // glass
                    sphereMaterial = std::make_shared<Dielectric>(1.5);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
                else
                {
                    // glass bubble
                    sphereMaterial = std::make_shared<Dielectric>(1.5);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                    world.add(std::make_shared<Sphere>(center, -0.18, sphereMaterial));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_shared<Sphere>(Vec3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Vec3(0.4, 0.2, 0.1));
    world.add(std::make_shared<Sphere>(Vec3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Vec3(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Sphere>(Vec3(4, 1, 0), 1.0, material3));

    return world;
}
