#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "surface.hpp"

#include <memory>

using std::shared_ptr;

class Vertex {
public:
    Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 tx) :
        p(p), n(n), tx(tx) { }
public:
    const glm::vec3 p;
    const glm::vec3 n;
    const glm::vec2 tx;
};


class Triangle : public Surface {
public:
    Triangle(Vertex a, Vertex b, Vertex c, MaterialPtr mat) :
        Surface(mat), centroid((a.p + b.p + c.p)/3.f),
        va(a), vb(b), vc(c), ba(a.p - b.p), ca(a.p - c.p) { }
    virtual ~Triangle() { }
    virtual bool isFinite() const { return true; }

    bool intersect(
        const glm::vec3 &eye,
        const glm::vec3 &dir,
        HitRecord &hr,
        const std::pair<float, float> &rng) const;
    virtual glm::vec3 getNorm(const glm::vec3 &p) const;
public:
    const glm::vec3 centroid;
    const Vertex va, vb, vc;
protected:
    // Basis vectors for triangle (avoid recalculating)
    glm::vec3 ba, ca;
};


// NB: Hack that a plane is a triangle of infinite size
class Plane : public Triangle {
public:
    Plane(Vertex a, Vertex b, Vertex c, MaterialPtr mat) : Triangle(a, b, c, mat) { }
    virtual ~Plane() { }
    bool isFinite() const { return false; }
    glm::vec3 getNorm(const glm::vec3 &p) const;
public:
    // Factory
    static shared_ptr<Plane> create(glm::vec3 a, glm::vec3 b, glm::vec3 c, MaterialPtr mtl);
};

#endif
