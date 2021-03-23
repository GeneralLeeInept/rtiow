#include "sphere_tree.h"

#include "core/rtiow.h"

static bool intersect(const Ray& r, const Vec3& center, double radiusSq, double& tMin, double& tMax)
{
    Vec3 oc = r.origin - center;
    double a = dot(r.direction, r.direction);
    double halfb = dot(oc, r.direction);
    double c = dot(oc, oc) - radiusSq;
    double discriminant = halfb * halfb - a * c;

    if (discriminant < 0)
    {
        return false;
    }

    double sqrtd = std::sqrt(discriminant);
    double rootA = (-halfb - sqrtd) / a;
    double rootB = (-halfb + sqrtd) / a;

    if (rootA > tMax || rootB < tMin)
    {
        return false;
    }

    tMin = std::max(tMin, rootA);
    tMax = std::min(tMax, rootB);

    return true;
}

static bool contains(const Vec3& centerA, double radiusA, const Vec3& centerB, double radiusB)
{
    double dist = length(centerB - centerA) + radiusB;
    return dist <= radiusA;
}

static void sphereUnion(const Vec3& centerA, double radiusA, const Vec3& centerB, double radiusB, Vec3& centerU, double& radiusU)
{
    if (contains(centerA, radiusA, centerB, radiusB))
    {
        centerU = centerA;
        radiusU = radiusA;
    }
    else if (contains(centerB, radiusB, centerA, radiusA))
    {
        centerU = centerB;
        radiusU = radiusB;
    }
    else
    {
        Vec3 v = centerB - centerA;
        double d = length(v);
        radiusU = (radiusA + radiusB + d) / 2.0;

        double t = (radiusU - radiusA) / d;
        centerU = lerp(centerA, centerB, t);
    }
}

bool SphereTree::hit(const Ray& r, double tMin, double tMax, HitRecord& hitRecord) const
{
    uint32_t nodeIndex = 0;
    const Node& node = nodes_[nodeIndex];
    return hit(nodes_[0], r, tMin, tMax, hitRecord);
}

bool SphereTree::hit(const Node& node, const Ray &r, double tMin, double tMax, HitRecord& hitRecord) const
{
    if (intersect(r, node.center, node.radiusSq, tMin, tMax))
    {
        if (node.leaf)
        {
            bool result = false;
            HitRecord test{};

            for (uint32_t i = 0; i < node.leafData.numPrimitives; ++i)
            {
                IHittable* object = objects_[i + node.leafData.firstPrimitive].get();

                if (object->hit(r, tMin, tMax, test))
                {
                    result = true;
                    tMax = test.t;
                    hitRecord = test;
                }
            }

            return result;
        }
        else
        {
            bool result = false;
            HitRecord test{};

            if (hit(nodes_[node.branchData.leftChild], r, tMin, tMax, test))
            {
                result = true;
                tMax = test.t;
                hitRecord = test;
            }

            if (hit(nodes_[node.branchData.rightChild], r, tMin, tMax, test))
            {
                result = true;
                tMax = test.t;
                hitRecord = test;
            }

            return result;
        }
    }
    else
    {
        return false;
    }
}

void SphereTreeBuilder::add(std::shared_ptr<IHittable> object, const Vec3& boundingSphereCenter, double boundingSphereRadius)
{
    Node node{};
    node.center = boundingSphereCenter;
    node.radius = boundingSphereRadius;
    node.objects.push_back(object);
    nodes_.push_back(std::move(node));
}

std::shared_ptr<SphereTree> SphereTreeBuilder::build()
{
    // Generate leaf nodes - minimum 2 objects per leaf node
    std::vector<Node> scratch;

    // Greedily find pairs which have the smallest union
    while (nodes_.size() > 1)
    {
        size_t bestA{};
        size_t bestB{};
        Vec3 bestCenter{};
        double bestRadius = std::numeric_limits<double>::max();

        for (size_t a = 0; a < nodes_.size() - 1; ++a)
        {
            for (size_t b = a + 1; b < nodes_.size(); ++b)
            {
                Vec3 centerU;
                double radiusU{};
                sphereUnion(nodes_[a].center, nodes_[a].radius, nodes_[b].center, nodes_[b].radius, centerU, radiusU);

                if (radiusU < bestRadius)
                {
                    bestA = a;
                    bestB = b;
                    bestCenter = centerU;
                    bestRadius = radiusU;
                }
            }
        }

        Node node{};
        node.center = bestCenter;
        node.radius = bestRadius;
        node.objects.insert(std::end(node.objects), std::begin(nodes_[bestA].objects), std::end(nodes_[bestA].objects));
        node.objects.insert(std::end(node.objects), std::begin(nodes_[bestB].objects), std::end(nodes_[bestB].objects));
        scratch.push_back(std::move(node));

        if (bestB < nodes_.size() - 1)
        {
            std::iter_swap(nodes_.begin() + bestB, nodes_.end() - 1);
            nodes_.resize(nodes_.size() - 1);
        }

        if (bestA < nodes_.size() - 1)
        {
            std::iter_swap(nodes_.begin() + bestA, nodes_.end() - 1);
            nodes_.resize(nodes_.size() - 1);
        }
    }

    if (nodes_.size() > 0)
    {
        // Find best leaf to which to add the last object
        size_t bestA{};
        Vec3 bestCenter{};
        double bestRadius = std::numeric_limits<double>::max();

        for (size_t a = 0; a < scratch.size(); ++a)
        {
            Vec3 centerU;
            double radiusU{};
            sphereUnion(scratch[a].center, scratch[a].radius, nodes_[0].center, nodes_[0].radius, centerU, radiusU);

            if (radiusU < bestRadius)
            {
                bestA = a;
                bestCenter = centerU;
                bestRadius = radiusU;
            }
        }

        scratch[bestA].center = bestCenter;
        scratch[bestA].radius = bestRadius;
        scratch[bestA].objects.insert(std::end(scratch[bestA].objects), std::begin(nodes_[0].objects), std::end(nodes_[0].objects));
        nodes_.clear();
    }

    // Now draw the rest of the owl
    std::vector<size_t> treenodes;

    for (size_t i = 0; i < scratch.size(); ++i)
    {
        treenodes.push_back(i);
    }

    while (treenodes.size() > 1)
    {
        // Find best pair of nodes to create a new branch node from
        size_t bestA{};
        size_t bestB{};
        Vec3 bestCenter{};
        double bestRadius = std::numeric_limits<double>::max();

        for (size_t a = 0; a < treenodes.size() - 1; ++a)
        {
            for (size_t b = a + 1; b < treenodes.size(); ++b)
            {
                Vec3 centerU;
                double radiusU{};
                Node& nodeA = scratch[treenodes[a]];
                Node& nodeB = scratch[treenodes[b]];
                sphereUnion(nodeA.center, nodeA.radius, nodeB.center, nodeB.radius, centerU, radiusU);

                if (radiusU < bestRadius)
                {
                    bestA = a;
                    bestB = b;
                    bestCenter = centerU;
                    bestRadius = radiusU;
                }
            }
        }

        Node node{};
        node.center = bestCenter;
        node.radius = bestRadius;
        node.leftChild = treenodes[bestA];
        node.rightChild = treenodes[bestB];
        scratch.push_back(std::move(node));

        if (bestB < treenodes.size() - 1)
        {
            std::iter_swap(treenodes.begin() + bestB, treenodes.end() - 1);
        }
        treenodes.resize(treenodes.size() - 1);

        if (bestA < treenodes.size() - 1)
        {
            std::iter_swap(treenodes.begin() + bestA, treenodes.end() - 1);
        }
        treenodes.resize(treenodes.size() - 1);

        treenodes.push_back(scratch.size() - 1);
    }

    // Pack into hittable
    std::shared_ptr<SphereTree> tree = std::make_shared<SphereTree>();
    addToResult(scratch, treenodes[0], tree);
    return tree;
}

uint32_t SphereTreeBuilder::addToResult(std::vector<Node> treeNodes, size_t index, std::shared_ptr<SphereTree>& tree)
{
    uint32_t result = uint32_t(tree->nodes_.size());
    tree->nodes_.push_back(SphereTree::Node());
    Node& srcNode = treeNodes[index];
    tree->nodes_[result].center = srcNode.center;
    tree->nodes_[result].radiusSq = srcNode.radius * srcNode.radius * 1.01;   // TODO: Science the expansion factor

    if (srcNode.leftChild)
    {
        tree->nodes_[result].branchData.leftChild = addToResult(treeNodes, srcNode.leftChild, tree);
        tree->nodes_[result].branchData.rightChild = addToResult(treeNodes, srcNode.rightChild, tree);
    }
    else
    {
        tree->nodes_[result].leaf = true;
        tree->nodes_[result].leafData.firstPrimitive = uint32_t(tree->objects_.size());
        tree->nodes_[result].leafData.numPrimitives = uint32_t(srcNode.objects.size());
        tree->objects_.insert(std::end(tree->objects_), std::begin(srcNode.objects), std::end(srcNode.objects));
    }

    return result;
}
