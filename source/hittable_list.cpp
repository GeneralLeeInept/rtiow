#include "hittable_list.h"

void HittableList::clear()
{
    mObjects.clear();
}

void HittableList::add(std::unique_ptr<IHittable> object)
{
    mObjects.push_back(std::move(object));
}

bool HittableList::Hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const
{
    bool result = false;
    HitRecord test{};

    for (const auto& object : mObjects)
    {
        if (object->Hit(r, tMin, tMax, test))
        {
            result = true;
            tMax = test.t;
            hit = test;
        }
    }

    return result;
}
