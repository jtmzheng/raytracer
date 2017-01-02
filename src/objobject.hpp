#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#include "triangle.hpp"

#include <vector>
#include <memory>

using namespace std;

class Transform;

struct AABB {
    AABB() {
        x = make_pair(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());
        y = make_pair(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());
        z = make_pair(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());
    }

    void update(const glm::vec3 &p) {
        x.first = min(x.first, p.x);
        y.first = min(y.first, p.y);
        z.first = min(z.first, p.z);
        x.second = max(x.second, p.x);
        y.second = max(y.second, p.y);
        z.second = max(z.second, p.z);
    }

    pair<float, float> x;
    pair<float, float> y;
    pair<float, float> z;
};

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
};

#endif
