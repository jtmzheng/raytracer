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
    virtual bool intersect(const glm::vec3 &eye, const glm::vec3 &dir, HitRecord &hr) const = 0;
};

class Sphere : public Surface {
public:
    Sphere(glm::vec3 c, float rad) : c(c), rad(rad) { }
    virtual bool intersect(const glm::vec3 &eye, const glm::vec3 &dir, HitRecord &hr) const;
private:
    glm::vec3 c;
    float rad;
};
#endif
