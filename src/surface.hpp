#ifndef SURFACE_H
#define SURFACE_H
// TODO: Factor out material related code

#include <memory>
#include <glm/glm.hpp>

#include <iostream>

class Surface;

struct HitRecord {
    float t;
    glm::vec3 dir;
    glm::vec3 eye;
    std::shared_ptr<Surface> surf;
};


class Surface {
public:
    Surface(glm::vec3 kd, glm::vec3 ks, float p) : kd(kd), ks(ks), p(p) { }
    Surface() : kd(0.7, 0.7, 0.7), ks(0.7, 0.7, 0.7), p(10) { }
    virtual ~Surface() { }

    virtual bool intersect(
        const glm::vec3 &eye,
        const glm::vec3 &dir,
        HitRecord &hr,
        const std::pair<float, float> &rng) const = 0;

    // Get the normal to the surface at a point p on the surface
    virtual glm::vec3 getNorm(const glm::vec3 &p) const = 0;
    glm::vec3 getKd() const { return kd; }
    glm::vec3 getKs() const { return ks; }
    float getP() const { return p; }
private:
    glm::vec3 kd;
    glm::vec3 ks;
    float p;
};


class Sphere : public Surface {
public:
    Sphere(glm::vec3 kd, glm::vec3 ks, float p, glm::vec3 c, float rad) :
        Surface(kd, ks, p), c(c), rad(rad) { }
    virtual ~Sphere() { }

    bool intersect(
        const glm::vec3 &eye,
        const glm::vec3 &dir,
        HitRecord &hr,
        const std::pair<float, float> &rng) const;
    glm::vec3 getNorm(const glm::vec3 &p) const;
private:
    glm::vec3 c;
    float rad;
};


class Triangle : public Surface {
public:
    Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c) :
        Surface(), va(a), vb(b), vc(c), ba(a - b), ca(a - c) { }
    virtual ~Triangle() { }
    virtual bool isFinite() const { return true; }

    bool intersect(
        const glm::vec3 &eye,
        const glm::vec3 &dir,
        HitRecord &hr,
        const std::pair<float, float> &rng) const;
    glm::vec3 getNorm(const glm::vec3 &p) const;
private:
    // Positions of vertices for triangle
    glm::vec3 va, vb, vc;

    // Basis vectors for triangle (avoid recalculating)
    glm::vec3 ba, ca;
};


// NB: Hack that a plane is a triangle of infinite size
class Plane : public Triangle {
public:
    Plane(glm::vec3 a, glm::vec3 b, glm::vec3 c) : Triangle(a, b, c) { }
    virtual ~Plane() { }
    bool isFinite() const { return false; }
};
#endif
