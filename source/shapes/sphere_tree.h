#pragma once

#include "hittable.h"

#include <cstdint>
#include <memory>
#include <vector>

class SphereTree : public IHittable
{
    struct Node
    {
        Vec3 center;
        double radiusSq;
        bool leaf;

        union
        {
            struct Leaf
            {
                uint32_t firstPrimitive;
                uint32_t numPrimitives;
            } leafData;
            struct Branch
            {
                uint32_t leftChild;
                uint32_t rightChild;
            } branchData;
        };
    };

public:
    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hitRecord) const override;

private:
    friend class SphereTreeBuilder;

    bool hit(const Node& node, const Ray& r, double tMin, double tMax, HitRecord& hitRecord) const;

    std::vector<std::shared_ptr<IHittable>> objects_;
    std::vector<Node> nodes_;
};

class SphereTreeBuilder
{
public:
    void add(std::shared_ptr<IHittable> object, const Vec3& boundingSphereCenter, double boundingSphereRadius);
    std::shared_ptr<SphereTree> build();

private:
    struct Node
    {
        Vec3 center;
        double radius;
        size_t leftChild;
        size_t rightChild;
        std::vector<std::shared_ptr<IHittable>> objects;
    };

    uint32_t addToResult(std::vector<Node> treeNodes, size_t index, std::shared_ptr<SphereTree>& tree);

    std::vector<Node> nodes_;
};
