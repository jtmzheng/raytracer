#include "bvh.hpp"

#include <iostream>

using namespace std;

#define LEAF_SIZE 10

Bvh::Bvh(vector<shared_ptr<Triangle>> &mesh) :
    root(mesh.size() > LEAF_SIZE ?
        shared_ptr<BvhNode>(new BvhNode(mesh.begin(), mesh.end())) :
        shared_ptr<BvhLeaf>(new BvhLeaf(mesh.begin(), mesh.end()))) {
}

BvhNode::BvhNode(ForwardIt begin, ForwardIt end) {
    // Partition based on the centroid bounding box
    AABB pb;
    glm::vec3 centroid;
    uint count = 0;
    for (auto t = begin; t != end; t = next(t), count++) {
        // Dereference iterator, then deref pointer...
        auto &tri = *t;
        pb.update(tri->centroid);
        box.update(tri->va.p);
        box.update(tri->vb.p);
        box.update(tri->vc.p);
        centroid += tri->centroid;
    }

    centroid /= count;
    auto dx = pb.x.second - pb.x.first,
         dy = pb.y.second - pb.y.first,
         dz = pb.z.second - pb.z.first;

    auto it = std::partition(begin, end, [centroid, dx, dy, dz](const shared_ptr<Triangle> &t){
        if (dx > dy && dx > dz)
            return t->centroid.x < centroid.x;
        if (dy > dx && dy > dz)
            return t->centroid.y < centroid.y;
        else
            return t->centroid.z < centroid.z;
    });

    // Children are leaf nodes if number of triangles < LEAF_SIZE
    auto leftNodes = std::distance(begin, it),
         rightNodes = std::distance(it, end);

    if (leftNodes > LEAF_SIZE && rightNodes > 0) {
        left = make_shared<BvhNode>(begin, it);
    } else {
        left = make_shared<BvhLeaf>(begin, it);
    }

    if (rightNodes > LEAF_SIZE && leftNodes > 0) {
        right = make_shared<BvhNode>(it, end);
    } else {
        right = make_shared<BvhLeaf>(it, end);
    }
}

BvhLeaf::BvhLeaf(ForwardIt begin, ForwardIt end) : mesh(begin, end) {
    // Copies subrange of mesh in leaf node
    cout << "Leaf: " << mesh.size() << endl;
    for (auto t = begin; t != end; t = next(t)) {
        // Dereference iterator, then deref pointer...
        auto &tri = *t;
        box.update(tri->va.p);
        box.update(tri->vb.p);
        box.update(tri->vc.p);
    }
}

bool BvhNode::inBB(
    const glm::vec3 &eye,
    const glm::vec3 &dir,
    const std::pair<float, float> &rng) const
{
    // TODO: Check actual POI is positive
    auto tx = calcT(eye.x, dir.x, box.x);
    auto ty = calcT(eye.y, dir.y, box.y);
    auto tz = calcT(eye.z, dir.z, box.z);
    if (tx.first > ty.second || tx.first > tz.second) return false;
    if (ty.first > tx.second || ty.first > tz.second) return false;
    if (tz.first > ty.second || tz.first > tx.second) return false;

    // Check if valid t is within range
    if (max(max(tx.first, ty.first), tz.first) > rng.second) return false;
    if (min(min(tx.second, ty.second), tz.second) < rng.first) return false;
    return true;
}

bool BvhNode::intersect(
    const glm::vec3 &eye,
    const glm::vec3 &dir,
    HitRecord &minHr,
    const std::pair<float, float> &rng) const {
    if (!inBB(eye, dir, rng))
        return false;

    // NB: Each leaf node will check whether hr.t < minHr.t passed down
    // so we can safely delegate to the leaf nodes
    left->intersect(eye, dir, minHr, rng);
    right->intersect(eye, dir, minHr, rng);
    return minHr.surf != nullptr;
}

bool BvhLeaf::intersect(
    const glm::vec3 &eye,
    const glm::vec3 &dir,
    HitRecord &minHr,
    const std::pair<float, float> &rng) const
{
    if (mesh.size() == 0)
        return false;

    if (!BvhNode::inBB(eye, dir, rng))
        return false;

    for (const auto &triangle : mesh) {
        triangle->intersect(eye, dir, minHr, rng);
    }

    return minHr.surf != nullptr;
}
