#include "test_scenes.h"

#include "core/rng.h"
#include "materials/material.h"
#include "materials/texture.h"
#include "shapes/aa_rect.h"
#include "shapes/aabb_tree.h"
#include "shapes/animated_transform.h"
#include "shapes/box.h"
#include "shapes/camera_invisible.h"
#include "shapes/flip_normals.h"
#include "shapes/sphere.h"
#include "shapes/sphere_tree.h"
#include "shapes/transform.h"

#include <iostream>

namespace scenes
{

static std::shared_ptr<Sky> makeGradientSky()
{
    return std::make_shared<GradientSky>(Vec3(1.0, 1.0, 1.0), Vec3(0.5, 0.7, 1.0));
}

static std::shared_ptr<Sky> makeStandardSky(std::string_view skyhdri)
{
    if (skyhdri.empty())
    {
        return makeGradientSky();
    }

    std::shared_ptr<HdriSky> hdriSky = std::make_shared<HdriSky>();

    if (!hdriSky->load(skyhdri))
    {
        std::cerr << "Failed to load HDRI sky.\n";
        exit(EXIT_FAILURE);
    }

    return hdriSky;
}

Scene oneWeekend()
{
    Scene scene;
    Rng rng(15021972);

    auto ground_material = std::make_shared<Lambertian>(Vec3(0.8, 0.8, 0.5));
    scene.add(std::make_shared<Sphere>(Vec3(0, -1000, 0), 1000, ground_material));

    HittableList balls;

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            double chooseMat = rng();
            Vec3 center(a + 0.9 * rng(), 0.2, b + 0.9 * rng());

            if (length(center - Vec3(4, 0.2, 0)) > 0.9)
            {
                std::shared_ptr<IMaterial> sphereMaterial;

                if (chooseMat < 0.5)
                {
                    // diffuse
                    Vec3 albedo = rng.color() * rng.color();
                    sphereMaterial = std::make_shared<Lambertian>(albedo);
                    balls.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
                else if (chooseMat < 0.75)
                {
                    // metal
                    Vec3 albedo = rng.color(0.5, 1);
                    double fuzz = rng(0.0, 0.5);
                    sphereMaterial = std::make_shared<Metal>(albedo, fuzz);
                    balls.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
                else if (chooseMat < 0.88)
                {
                    // glass
                    sphereMaterial = std::make_shared<Dielectric>(1.5);
                    balls.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
                else
                {
                    // glass bubble
                    sphereMaterial = std::make_shared<Dielectric>(1.5);
                    balls.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                    balls.add(std::make_shared<Sphere>(center, -0.18, sphereMaterial));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    balls.add(std::make_shared<Sphere>(Vec3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Vec3(0.4, 0.2, 0.1));
    balls.add(std::make_shared<Sphere>(Vec3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Vec3(0.7, 0.6, 0.5), 0.0);
    balls.add(std::make_shared<Sphere>(Vec3(4, 1, 0), 1.0, material3));

    scene.add(std::make_shared<AabbTreeNode>(balls, 0, 1, rng));

    scene.cameraCreateInfo.position = Vec3(13, 2, 3);
    scene.cameraCreateInfo.target = Vec3(0, 0, 0);
    scene.cameraCreateInfo.vup = Vec3(0, 1, 0);
    scene.cameraCreateInfo.fovy = degToRad(30);
    scene.cameraCreateInfo.focalDistance = 10.0;
    scene.cameraCreateInfo.aperature = 0.1;

    scene.sky = makeGradientSky();

    return scene;
}

Scene ballsGalore(std::string_view skyhdri)
{
    Scene scene;
    Rng rng(15021972);

    auto checker = std::make_shared<CheckerTexture>(Vec3(0.2, 0.3, 0.1), Vec3(0.9, 0.9, 0.9));
    auto ground_material = std::make_shared<Lambertian>(checker);
    scene.add(std::make_shared<Sphere>(Vec3(0, -1000, 0), 1000, ground_material));

    SphereTreeBuilder builder{};

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            double chooseMat = rng();
            Vec3 center(a + 0.9 * rng(), 0.2, b + 0.9 * rng());

            if (length(center - Vec3(4, 0.2, 0)) > 0.9)
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
                    double fuzz = rng(0.0, 0.5);
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

    scene.add(builder.build());

    scene.cameraCreateInfo.position = Vec3(13, 2, 3);
    scene.cameraCreateInfo.target = Vec3(0, 0, 0);
    scene.cameraCreateInfo.vup = Vec3(0, 1, 0);
    scene.cameraCreateInfo.fovy = degToRad(30);
    scene.cameraCreateInfo.focalDistance = 10.0;
    scene.cameraCreateInfo.aperature = 0.1;

    scene.sky = makeStandardSky(skyhdri);

    return scene;
}

Scene cornellBox()
{
    // Approximately based on http://www.graphics.cornell.edu/online/box/data.html
    constexpr double xmin = 0;
    constexpr double xmax = 550;
    constexpr double ymin = 0;
    constexpr double ymax = 550;
    constexpr double zmin = 0;
    constexpr double zmax = 560;

    Scene scene;

    std::shared_ptr<IMaterial> red = std::make_shared<Lambertian>(Vec3(0.6, 0.0, 0.0));
    std::shared_ptr<IMaterial> green = std::make_shared<Lambertian>(Vec3(0.0, 0.5, 0.0));
    std::shared_ptr<IMaterial> white = std::make_shared<Lambertian>(Vec3(0.7, 0.7, 0.7));
    std::shared_ptr<IMaterial> light = std::make_shared<LightSource>(Vec3(18.4, 15.6, 8.0));

    // Floor
    scene.add(std::make_shared<RectangleXZ>(xmin, xmax, zmin, zmax, ymin, white));

    // Ceiling
    scene.add(std::make_shared<RectangleXZ>(xmin, xmax, zmin, zmax, ymax, white));

    // Back wall
    scene.add(std::make_shared<RectangleXY>(xmin, xmax, ymin, ymax, zmax, white));

    // Front wall (invisible to camera)
    scene.add(std::make_shared<CameraInvisible>(std::make_shared<RectangleXY>(xmin, xmax, ymin, ymax, zmin, white)));

    // Left wall
    scene.add(std::make_shared<RectangleYZ>(ymin, ymax, zmin, zmax, xmax, red));

    // Right wall
    scene.add(std::make_shared<RectangleYZ>(ymin, ymax, zmin, zmax, xmin, green));

    // Light
    scene.add(std::make_shared<FlipNormals>(std::make_shared<RectangleXZ>(213, 343, 227, 332, 549.5, light)));

    // Tall block
    constexpr Vec3 tallBoxDims = { 165, 330, 167 };
    Mat4 xform = glm::translate(Vec3(380, 165, 400)) * glm::rotate(60.0, Vec3(0, 1, 0));
    scene.add(std::make_shared<Transform>(xform, std::make_shared<Box>(tallBoxDims, white)));

    // Short block
    constexpr Vec3 shortBoxDims = { 167, 165, 165 };
    xform = glm::translate(Vec3(200, 82.5, 280)) * glm::rotate(-60.0, Vec3(0, 1, 0));
    scene.add(std::make_shared<Transform>(xform, std::make_shared<Box>(shortBoxDims, white)));

    // Glass ball
    scene.add(std::make_shared<Sphere>(Vec3(200, 215, 280), 50, std::make_shared<Dielectric>(1.5)));

    // Camera
    scene.cameraCreateInfo.position = Vec3(278, 273, -800);
    scene.cameraCreateInfo.target = Vec3(278, 273, 0);
    scene.cameraCreateInfo.vup = Vec3(0, 1, 0);
    scene.cameraCreateInfo.fovy = degToRad(35);
    scene.cameraCreateInfo.focalDistance = 800.0;
    scene.cameraCreateInfo.aperature = 0.1;

    // Black environment
    scene.sky = std::make_shared<ConstantColorSky>(Vec3(0, 0, 0));

    return scene;
}

Scene boxTest(std::string_view skyhdri)
{
    Scene scene;
    std::shared_ptr<IMaterial> grey = std::make_shared<Lambertian>(Vec3(0.5, 0.5, 0.5));
    Mat4 xform = glm::translate(Vec3(0, -2.0, 0)) * glm::rotate(30.0, Vec3(0, 1, 0));
    scene.add(std::make_shared<Transform>(xform, std::make_shared<Box>(Vec3(2.0, 2.0, 2.0), grey)));

    scene.cameraCreateInfo.position = Vec3(0, 0, -10);
    scene.cameraCreateInfo.target = Vec3(0, 0, 0);
    scene.cameraCreateInfo.vup = Vec3(0, 1, 0);
    scene.cameraCreateInfo.fovy = degToRad(35);
    scene.cameraCreateInfo.focalDistance = 10.0;
    scene.cameraCreateInfo.aperature = 0.1;

    scene.sky = makeStandardSky(skyhdri);

    return scene;
}

Scene animTest(std::string_view skyhdri)
{
    Scene scene;

    scene.sky = makeStandardSky(skyhdri);

    scene.cameraCreateInfo.position = Vec3(0, 3, -10);
    scene.cameraCreateInfo.target = Vec3(0, 0, 0);
    scene.cameraCreateInfo.vup = Vec3(0, 1, 0);
    scene.cameraCreateInfo.fovy = degToRad(35);
    scene.cameraCreateInfo.focalDistance = 10.0;
    scene.cameraCreateInfo.aperature = 0.1;
    scene.cameraCreateInfo.timeBegin = 0.5;
    scene.cameraCreateInfo.timeEnd = 0.6;

    scene.add(std::make_shared<RectangleXZ>(-20, 20, -20, 20, -5, std::make_shared<Lambertian>(Vec3(0.5, 0.5, 0.5))));

    auto box = std::make_shared<Box>(Vec3(1, 1, 1), std::make_shared<Lambertian>(Vec3(0.6, 0, 0)));
    auto boxTransform = std::make_shared<AnimatedTransform>(box);
    Quat q1{};
    Quat q2 = glm::angleAxis(degToRad(90), normalize(Vec3(1, 1, 1)));
    boxTransform->addKeyFrame({ q2, Vec3(0, 0, 0), 0.0 });
    boxTransform->addKeyFrame({ q2, Vec3(0, -3, 0), 1.0 });
    scene.add(boxTransform);

    auto sphere = std::make_shared<Sphere>(Vec3(), 1.0, std::make_shared<Lambertian>(Vec3(0, 0.5, 0)));
    auto sphereTransform = std::make_shared<AnimatedTransform>(sphere);
    sphereTransform->addKeyFrame({Quat(), Vec3(-8, 0, 4), 0 });
    sphereTransform->addKeyFrame({Quat(), Vec3(8, 0, 4), 1 });
    scene.add(sphereTransform);

    return scene;
}

Scene texturedSphere(std::string_view skyhdri)
{
    Scene scene;

    scene.sky = makeStandardSky(skyhdri);

    scene.cameraCreateInfo.position = Vec3(3, -3, 3);
    scene.cameraCreateInfo.target = Vec3(0, 0, 0);
    scene.cameraCreateInfo.vup = Vec3(0, 1, 0);
    scene.cameraCreateInfo.fovy = degToRad(30);
    scene.cameraCreateInfo.focalDistance = 5.0;
    scene.cameraCreateInfo.aperature = 0.1;

    auto texture = std::make_shared<ImageTexture>(R"(R:\assets\textures\earthmap.png)");
    auto material = std::make_shared<Lambertian>(texture);
    auto sphere = std::make_shared<Sphere>(Vec3(), 1.0, material);
    scene.add(sphere);

    return scene;
}

Scene noiseTextureTest()
{
    Scene scene;

    scene.sky = makeGradientSky();

    scene.cameraCreateInfo.position = Vec3(0, 1, -10);
    scene.cameraCreateInfo.target = Vec3(0, 0, 0);
    scene.cameraCreateInfo.vup = Vec3(0, 1, 0);
    scene.cameraCreateInfo.fovy = degToRad(30);
    scene.cameraCreateInfo.focalDistance = 5.0;
    scene.cameraCreateInfo.aperature = 0;

    auto texture = std::make_shared<NoiseTexture>(8);
    auto material = std::make_shared<Lambertian>(texture);
    scene.add(std::make_shared<Sphere>(Vec3(0, 1, 0), 1, material));
    scene.add(std::make_shared<Sphere>(Vec3(0, -50, 0), 50, material));

    return scene;
}

Scene theNextWeek()
{
    Scene scene;
    Rng rng(15021972);

    HittableList boxes1;
    auto ground = std::make_shared<Lambertian>(Vec3(0.48, 0.83, 0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++)
    {
        for (int j = 0; j < boxes_per_side; j++)
        {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = rng(1.0, 101.0);
            auto z1 = z0 + w;

            boxes1.add(std::make_shared<Box>(Vec3(x0, y0, z0), Vec3(x1, y1, z1), ground));
        }
    }

    scene.add(std::make_shared<AabbTreeNode>(boxes1, 0, 1, rng));

    auto light = std::make_shared<LightSource>(Vec3(7, 7, 7));
    scene.add(std::make_shared<FlipNormals>(std::make_shared<RectangleXZ>(123, 423, 147, 412, 554, light)));

    auto moving_sphere_material = std::make_shared<Lambertian>(Vec3(0.7, 0.3, 0.1));
    auto sphere = std::make_shared<Sphere>(Vec3(400, 400, 200), 50, moving_sphere_material);
    auto spherexform = std::make_shared<AnimatedTransform>(sphere);
    spherexform->addKeyFrame({Quat(), Vec3(), 0.0});
    spherexform->addKeyFrame({Quat(), Vec3(30,0,0), 1.0});
    scene.add(spherexform);

    scene.add(std::make_shared<Sphere>(Vec3(260, 150, 45), 50, std::make_shared<Dielectric>(1.5)));
    scene.add(std::make_shared<Sphere>(Vec3(0, 150, 145), 50, std::make_shared<Metal>(Vec3(0.8, 0.8, 0.9), 1.0)));

#if 0
    auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001, color(1,1,1)));
#endif

    auto emat = std::make_shared<Lambertian>(std::make_shared<ImageTexture>(R"(R:\assets\textures\earthmap.png)"));
    scene.add(std::make_shared<Sphere>(Vec3(400,200,400), 100, emat));


#if 0
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(point3(220,280,300), 80, make_shared<lambertian>(pertext)));
#endif

    HittableList boxes2;
    auto white = std::make_shared<Lambertian>(Vec3(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++)
    {
        boxes2.add(std::make_shared<Sphere>(Vec3(rng(0.0, 165.0), rng(0.0, 165.0), rng(0.0, 165.0)), 10.0, white));
    }

    Mat4 boxes2xform = glm::translate(Vec3(-100, 270, 395)) * glm::rotate(degToRad(15), Vec3(0, 1, 0));
    scene.add(std::make_shared<Transform>(boxes2xform, std::make_shared<AabbTreeNode>(boxes2, 0, 1, rng)));

    scene.sky = std::make_shared<ConstantColorSky>(Vec3());
    scene.cameraCreateInfo.position = Vec3(478, 278, -600);
    scene.cameraCreateInfo.target = Vec3(278, 278, 0);
    scene.cameraCreateInfo.vup = Vec3(0, 1, 0);
    scene.cameraCreateInfo.fovy = degToRad(40.0);
    scene.cameraCreateInfo.focalDistance = 600;
    scene.cameraCreateInfo.aperature = 0.1;
    scene.cameraCreateInfo.timeBegin = 0.0;
    scene.cameraCreateInfo.timeEnd = 1.0;

    return scene;
}
}
