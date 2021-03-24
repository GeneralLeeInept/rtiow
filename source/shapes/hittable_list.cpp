#include "hittable_list.h"

#include "core/hit_record.h"

void HittableList::clear()
{
    objects_.clear();
}

void HittableList::add(std::shared_ptr<IHittable> object)
{
    objects_.push_back(object);
}

bool HittableList::hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const
{
    bool result = false;
    HitRecord test{};

    for (const auto& object : objects_)
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

bool HittableList::boundingBox(double startTime, double endTime, Aabb& bbox) const
{
    if (objects_.empty())
    {
        return false;
    }

    bool first = true;

    for (const auto& object : objects_)
    {
        Aabb objectBbox{};

        if (!object->boundingBox(startTime, endTime, objectBbox))
        {
            return false;
        }

        if (first)
        {
            bbox = objectBbox;
            first = false;
        }
        else
        {
            bbox = bbox.makeUnion(objectBbox);
        }
    }

    return true;
}
