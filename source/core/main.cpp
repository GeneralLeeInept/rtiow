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
#include "scenes/test_scenes.h"
#include "shapes/hittable_list.h"
#include "shapes/sphere.h"
#include "shapes/sphere_tree.h"

Vec3 rayColor(const Ray& r, const Scene& scene, Rng& rng, int depth)
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
            return hit.material->Emitted(hit) + hit.material->Albedo() * rayColor(scattered, scene, rng, depth - 1);
        }
        else
        {
            return hit.material->Emitted(hit);
        }
    }
    else
    {
        return scene.sky->Sample(r.direction);
    }
}

struct Job
{
    Job(uint32_t width, uint32_t height)
        : image(width, height)
    {
    }

    void run(const Scene& scene, int numPasses, int maxDepth)
    {
        thread_ = std::thread(&Job::threadFunc, this, scene, numPasses, maxDepth);
    }

    void wait()
    {
        thread_.join();
    }

    void threadFunc(const Scene& scene, int numPasses, int maxDepth)
    {
        for (int y = int(image.height()); --y >= 0;)
        {
            for (int x = 0; x < int(image.width()); ++x)
            {
                Vec3 color{};

                for (int s = 0; s < numPasses; ++s)
                {
                    double u = double(x + rng_()) / image.width();
                    double v = double(y + rng_()) / image.height();
                    Ray r = scene.camera->createRay(rng_, u, v);
                    color += rayColor(r, scene, rng_, maxDepth);
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
    args.sceneId = UINT32_MAX;

    if (!parseCommandLine(argc, argv, args))
    {
        exit(EXIT_FAILURE);
    }

    double aspectRatio = double(args.imageWidth) / double(args.imageHeight);

    Image image{ args.imageWidth, args.imageHeight };
    Scene scene{};

    switch (args.sceneId)
    {
        case 0:
        {
            scene = scenes::ballsGalore();
            Vec3 cameraPos(13, 2, 3);
            Vec3 cameraTarget(0, 0, 0);
            double focalLength = 10.0;
            double aperature = 0.1;
            scene.camera = std::make_shared<Camera>(cameraPos, cameraTarget, Vec3(0, 1, 0), degToRad(30), aspectRatio, aperature, focalLength);

            if (args.hdriSkyPath.empty())
            {
                scene.sky = std::make_shared<GradientSky>(Vec3(1.0, 1.0, 1.0), Vec3(0.5, 0.7, 1.0));
            }
            else
            {
                std::shared_ptr<HdriSky> hdriSky = std::make_shared<HdriSky>();

                if (!hdriSky->load(args.hdriSkyPath))
                {
                    std::cerr << "Failed to load HDRI sky.\n";
                    exit(EXIT_FAILURE);
                }

                scene.sky = hdriSky;
            }

            break;
        }
        default:
        case 1:
        {
            scene = scenes::cornellBox();
            Vec3 cameraPos(278, 273, -800);
            Vec3 cameraTarget(278, 273, 0);
            double focalLength = 800.0;
            double aperature = 0.1;
            scene.camera = std::make_shared<Camera>(cameraPos, cameraTarget, Vec3(0, 1, 0), degToRad(35), aspectRatio, aperature, focalLength);
            scene.sky = std::make_shared<ConstantColorSky>(Vec3(0, 0, 0));
            break;
        }
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
        jobs[i].run(scene, passesPerJob + 1, args.maxDepth);
    }

    for (uint32_t i = extraPasses; i < args.numJobs; ++i)
    {
        jobs[i].run(scene, passesPerJob, args.maxDepth);
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
