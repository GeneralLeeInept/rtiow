#include "test_scenes.h"

#include "core/rng.h"
#include "shapes/box.h"
#include "shapes/sphere.h"
#include "shapes/sphere_tree.h"

namespace scenes
{

Scene ballsGalore()
{
    Scene scene;
    Rng rng(15021972);

    auto ground_material = std::make_shared<Lambertian>(Vec3(0.8, 0.8, 0.5));
    scene.add(std::make_shared<Sphere>(Vec3(0, -1000, 0), 1000, ground_material));

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

    scene.add(builder.build());

    return scene;
}

Scene cornellBox()
{
    // Approximately based on http://www.graphics.cornell.edu/online/box/data.html
    Scene scene;

    std::shared_ptr<IMaterial> red = std::make_shared<Lambertian>(Vec3(0.6, 0.0, 0.0));
    std::shared_ptr<IMaterial> green = std::make_shared<Lambertian>(Vec3(0.0, 0.5, 0.0));
    std::shared_ptr<IMaterial> white = std::make_shared<Lambertian>(Vec3(0.7, 0.7, 0.7));

    // Floor
    scene.add(std::make_shared<RectangleXZ>(0, 550, 0, 560, 0, white));

    // Ceiling
    scene.add(std::make_shared<RectangleXZ>(0, 550, 0, 560, 550, white));

    // Back wall
    scene.add(std::make_shared<RectangleXY>(0, 550, 0, 550, 560, white));

    // Right wall
    scene.add(std::make_shared<RectangleYZ>(0, 550, 0, 560, 0, green));

    // Left wall
    scene.add(std::make_shared<RectangleYZ>(0, 550, 0, 560, 550, red));

    return scene;
}

}
