#pragma once

#include "shapes/hittable_list.h"
#include "core/sky.h"
#include "camera/camera.h"

class Scene : public HittableList
{
public:
    std::shared_ptr<Sky> sky;
    std::shared_ptr<Camera> camera;
};
