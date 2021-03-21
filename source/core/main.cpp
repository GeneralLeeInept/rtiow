#include <iostream>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <thread>

#include "camera/camera.h"
#include "core/command_line.h"
#include "core/image.h"
#include "core/ray.h"
#include "core/rng.h"
#include "core/sky.h"
#include "core/vec3.h"
#include "core/rtiow.h"
#include "materials/material.h"
#include "shapes/hittable_list.h"
#include "shapes/sphere.h"
#include "shapes/sphere_tree.h"

Vec3 rayColor(const Ray& r, const HittableList& scene, const std::shared_ptr<Sky>& sky, Rng& rng, int depth)
{
    if (depth == 0)
    {
        return Vec3{};
    }

    HitRecord hit{};
    bool b = scene.hit(r, 0.001, std::numeric_limits<double>::infinity(), hit);

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
        return sky->Sample(r.direction);
    }
}

static HittableList randomScene();

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

    void run(const HittableList& scene, const Camera& camera, const std::shared_ptr<Sky> sky, int numPasses, int maxDepth)
    {
        thread_ = std::thread(&Job::threadFunc, this, scene, camera, sky, numPasses, maxDepth);
    }

    void wait()
    {
        thread_.join();
    }

    void threadFunc(const HittableList& scene, const Camera& camera, const std::shared_ptr<Sky> sky, int numPasses, int maxDepth)
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
                    color += rayColor(r, scene, sky, rng_, maxDepth);
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
    args.numJobs = std::thread::hardware_concurrency();
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

    HittableList scene = randomScene();

    std::shared_ptr<Sky> sky{};

    if (args.hdriSkyPath.empty())
    {
        sky = std::make_shared<GradientSky>(Vec3(1.0, 1.0, 1.0), Vec3(0.5, 0.7, 1.0));
    }
    else
    {
        std::shared_ptr<HdriSky> hdriSky = std::make_shared<HdriSky>();

        if (!hdriSky->load(args.hdriSkyPath))
        {
            std::cerr << "Failed to load HDRI sky.\n";
            exit(EXIT_FAILURE);
        }

        sky = hdriSky;
    }

    if (args.numJobs == 0)
    {
        args.numJobs = 1;
    }

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

HittableList randomScene()
{
    HittableList world;
    Rng rng(15021972);

    auto ground_material = std::make_shared<Lambertian>(Vec3(0.8, 0.8, 0.5));
    world.add(std::make_shared<Sphere>(Vec3(0, -1000, 0), 1000, ground_material));

    SphereTreeBuilder builder{};

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            double chooseMat = rng();
            Vec3 center(a + 0.9 * rng(), 0.2, b + 0.9 * rng());

            if ((center - Vec3(4, 0.2, 0)).length() > 0.9)
            {
                std::shared_ptr<IMaterial> sphereMaterial;

                if (chooseMat < 0.5)
                {
                    // diffuse
                    Vec3 albedo = rng.color() * rng.color();
                    sphereMaterial = std::make_shared<Lambertian>(albedo);
                    builder.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial), center, 0.2);
                }
                else if (chooseMat < 0.75)
                {
                    // metal
                    Vec3 albedo = rng.color(0.5, 1);
                    double fuzz = rng(0, 0.5);
                    sphereMaterial = std::make_shared<Metal>(albedo, fuzz);
                    builder.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial), center, 0.2);
                }
                else if (chooseMat < 0.88)
                {
                    // glass
                    sphereMaterial = std::make_shared<Dielectric>(1.5);
                    builder.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial), center, 0.2);
                }
                else
                {
                    // glass bubble
                    sphereMaterial = std::make_shared<Dielectric>(1.5);
                    builder.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial), center, 0.2);
                    builder.add(std::make_shared<Sphere>(center, -0.18, sphereMaterial), center, 0.18);
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    builder.add(std::make_shared<Sphere>(Vec3(0, 1, 0), 1.0, material1), Vec3(0, 1, 0), 1.0);

    auto material2 = std::make_shared<Lambertian>(Vec3(0.4, 0.2, 0.1));
    builder.add(std::make_shared<Sphere>(Vec3(-4, 1, 0), 1.0, material2), Vec3(-4, 1, 0), 1.0);

    auto material3 = std::make_shared<Metal>(Vec3(0.7, 0.6, 0.5), 0.0);
    builder.add(std::make_shared<Sphere>(Vec3(4, 1, 0), 1.0, material3), Vec3(4, 1, 0), 1.0);

    world.add(builder.build());

    return world;
}
