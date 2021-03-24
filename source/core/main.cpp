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
            return hit.material->Emitted(hit) + hit.material->Albedo(hit) * rayColor(scattered, scene, rng, depth - 1);
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
            scene = scenes::oneWeekend();
            break;
        }
        case 1:
        {
            scene = scenes::ballsGalore(args.hdriSkyPath);
            break;
        }
        case 2:
        {
            scene = scenes::cornellBox();
            break;
        }
        case 3:
        {
            scene = scenes::boxTest(args.hdriSkyPath);
            break;
        }
        case 4:
        {
            scene = scenes::animTest(args.hdriSkyPath);
            break;
        }
        case 5:
        {
            scene = scenes::texturedSphere(args.hdriSkyPath);
            break;
        }
        case 6:
        {
            scene = scenes::noiseTextureTest();
            break;
        }
        default:
        case 7:
        {
            scene = scenes::theNextWeek();
            break;
        }
    }

    scene.camera = std::make_shared<Camera>(scene.cameraCreateInfo, aspectRatio);

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
