#ifndef BVH_H
#define BVH_H

#include "surface.hpp"
#include "triangle.hpp"
#include "aabb.hpp"

#include <vector>
#include <memory>

typedef std::vector<std::shared_ptr<Triangle>>::iterator ForwardIt;

class BvhNode : public Object {
public:
    BvhNode(ForwardIt begin, ForwardIt end);
    BvhNode() { }
    bool intersect(
        const glm::vec3 &eye,
        const glm::vec3 &dir,
        HitRecord &hr,
        const std::pair<float, float> &rng) const;
protected:
    AABB box;
    bool inBB(
        const glm::vec3 &eye,
        const glm::vec3 &dir,
        const std::pair<float, float> &rng) const;
private:
    std::shared_ptr<BvhNode> left;
    std::shared_ptr<BvhNode> right;
};


class BvhLeaf : public BvhNode {
public:
    BvhLeaf(ForwardIt begin, ForwardIt end);
    bool intersect(
        const glm::vec3 &eye,
        const glm::vec3 &dir,
        HitRecord &hr,
        const std::pair<float, float> &rng) const;
private:
    std::vector<std::shared_ptr<Triangle>> mesh;
};


// Bounding Volume Hierarchy
class Bvh : public Object {
public:
    // Currently only supports triangle meshes
    // NB: std::partition is used, so mesh will be mutated, but this is _okay_
    Bvh(std::vector<std::shared_ptr<Triangle>> &mesh);
    bool intersect(
        const glm::vec3 &eye,
        const glm::vec3 &dir,
        HitRecord &hr,
        const std::pair<float, float> &rng) const { return root->intersect(eye, dir, hr, rng); }
private:
    std::shared_ptr<BvhNode> root;
};
#endif
