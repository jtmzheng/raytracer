#ifndef SURFACE_H
#define SURFACE_H

#include <memory>
#include <glm/glm.hpp>

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
    virtual bool intersect(const glm::vec3 &eye, const glm::vec3 &dir, HitRecord &hr) const = 0;

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
    bool intersect(const glm::vec3 &eye, const glm::vec3 &dir, HitRecord &hr) const;
    glm::vec3 getNorm(const glm::vec3 &p) const;
private:
    glm::vec3 c;
    float rad;
};
#endif
