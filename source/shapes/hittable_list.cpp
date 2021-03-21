#include "hittable_list.h"

void HittableList::clear()
{
    mObjects.clear();
}

void HittableList::add(std::shared_ptr<IHittable> object)
{
    mObjects.push_back(object);
}

void HittableList::setSky(std::shared_ptr<Sky> sky)
{
    mSky = sky;
}

std::shared_ptr<const Sky> HittableList::sky() const
{
    return mSky;
}

bool HittableList::hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const
{
    bool result = false;
    HitRecord test{};

    for (const auto& object : mObjects)
    {
        if (object->hit(r, tMin, tMax, test))
        {
            result = true;
            tMax = test.t;
            hit = test;
        }
    }

    return result;
}
