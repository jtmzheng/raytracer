#ifndef SURFACE_H
#define SURFACE_H

#include <memory>
#include <glm/glm.hpp>

#include "material.hpp"

class Surface;

struct HitRecord {
    float t;
    glm::vec3 dir;
    glm::vec3 eye;
    std::shared_ptr<Surface> surf;
};

class Surface {
public:
    Surface(std::shared_ptr<Material> mat) : mat(mat) { }
    virtual ~Surface() { }

    virtual bool intersect(
        const glm::vec3 &eye,
        const glm::vec3 &dir,
        HitRecord &hr,
        const std::pair<float, float> &rng) const = 0;

    // Get the normal to the surface at a point p on the surface
    virtual glm::vec3 getNorm(const glm::vec3 &p) const = 0;
    const Material& getMaterial() const { return *mat.get(); }
private:
    std::shared_ptr<Material> mat;
};


class Sphere : public Surface {
public:
    Sphere(glm::vec3 c, float rad, std::shared_ptr<Material> mat) :
        Surface(mat), c(c), rad(rad) { }
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
    Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, std::shared_ptr<Material> mat) :
        Surface(mat), va(a), vb(b), vc(c), ba(a - b), ca(a - c) { }
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
    Plane(glm::vec3 a, glm::vec3 b, glm::vec3 c, std::shared_ptr<Material> mat) : Triangle(a, b, c, mat) { }
    virtual ~Plane() { }
    bool isFinite() const { return false; }
};
#endif
