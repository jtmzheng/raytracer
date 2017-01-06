#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#include "triangle.hpp"
#include "aabb.hpp"
#include "bvh.hpp"

#include <vector>
#include <memory>

using namespace std;

class Transform;

class ObjObject : public Object {
public:
    ObjObject() { }
    virtual ~ObjObject() { }
    bool intersect(
        const glm::vec3 &eye,
        const glm::vec3 &dir,
        HitRecord &hr,
        const std::pair<float, float> &rng) const;

    // Load each shape in obj file as a ObjObject
    static vector<shared_ptr<ObjObject>> loadFromFile(string file, string base, const Transform &transform);
private:
    vector<shared_ptr<Triangle>> triangles;
    AABB box;

    // This is to allow lazy loading... don't judge :P
    shared_ptr<Bvh> bvh;
};

#endif
