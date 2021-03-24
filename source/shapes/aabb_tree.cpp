#include "aabb_tree.h"

#include "core/hit_record.h"
#include "shapes/hittable_list.h"

#include <algorithm>
#include <iostream>

AabbTreeNode::AabbTreeNode(const HittableList& list, double timeStart, double timeEnd, Rng& rng)
    : AabbTreeNode(list.objects(), 0, list.objects().size(), timeStart, timeEnd, rng)
{
}

AabbTreeNode::AabbTreeNode(const std::vector<std::shared_ptr<IHittable>>& srcobjects, size_t start, size_t end, double timeStart, double timeEnd, Rng& rng)
{
    std::vector<std::shared_ptr<IHittable>> objects = srcobjects;
    size_t object_span = end - start;
    int axis = rng.randomInt(0, 2);

    auto comparator = [axis](const std::shared_ptr<IHittable>& a, const std::shared_ptr<IHittable>& b) -> bool
    {
        Aabb boxA{};
        Aabb boxB{};

        if (!a->boundingBox(0, 0, boxA) || !b->boundingBox(0, 0, boxB))
        {
            return false;
        }

        return (boxA.mins[axis] + boxA.maxs[axis]) < (boxB.mins[axis] + boxB.maxs[axis]);
    };

    if (object_span == 1)
    {
        left_ = right_ = objects[start];
    }
    else if (object_span == 2)
    {
        if (comparator(objects[start], objects[start+1]))
        {
            left_ = objects[start];
            right_ = objects[start+1];
        }
        else
        {
            left_ = objects[start+1];
            right_ = objects[start];
        }
    }
    else
    {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);
        auto mid = start + object_span / 2;
        left_ = std::make_shared<AabbTreeNode>(objects, start, mid, timeStart, timeEnd, rng);
        right_ = std::make_shared<AabbTreeNode>(objects, mid, end, timeStart, timeEnd, rng);
    }

    Aabb bboxLeft{};

    if (!left_->boundingBox(timeStart, timeEnd, bboxLeft))
    {
        std::cerr << "No bounding box in AabbTreeNode constructor\n";
        exit(EXIT_FAILURE);
    }

    Aabb bboxRight{};

    if (!right_->boundingBox(timeStart, timeEnd, bboxRight))
    {
        std::cerr << "No bounding box in AabbTreeNode constructor\n";
        exit(EXIT_FAILURE);
    }

    bounds_ = bboxLeft.makeUnion(bboxRight);
}

bool AabbTreeNode::hit(const Ray& r, double tMin, double tMax, HitRecord& hitRecord) const
{
    if (!bounds_.hit(r, tMin, tMax))
    {
        return false;
    }

    bool hitLeft = left_->hit(r, tMin, tMax, hitRecord);
    bool hitRight = right_->hit(r, tMin, hitLeft ? hitRecord.t : tMax, hitRecord);
    return hitLeft || hitRight;
}

bool AabbTreeNode::boundingBox(double timeStart, double timeEnd, Aabb& bbox) const
{
    bbox = bounds_;
    return true;
}
