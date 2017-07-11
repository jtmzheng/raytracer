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

    // A HitRecord should _never_ outlive surface it hits
    const Surface *surf;
    glm::vec3 norm;
    glm::vec2 uv;
};

// An Object instance is something a ray can intersect
class Object {
public:
    Object() { }
    virtual ~Object() { }

    // intersect _must_ set hr.surf and h.t iff intersection found
    virtual bool intersect(
        const glm::vec3 &eye,
        const glm::vec3 &dir,
        HitRecord &hr,
        const std::pair<float, float> &rng) const = 0;
};


// A Surface instance is geometry that can be intersected
class Surface : public Object {
public:
    Surface(std::shared_ptr<Material> mat) : mat(mat) { }
    virtual ~Surface() { }

    // Get the normal to the surface at a point p on the surface
    // NB: Very weird... each in coordinate system of derived class
    // eg) triangle is barycentric, sphere is world
    // TODO: FIX THIS
    virtual glm::vec3 getNorm(const glm::vec3 &p) const = 0;
    virtual glm::vec2 getUV(const glm::vec3 &p) const = 0;
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
    glm::vec2 getUV(const glm::vec3 &p) const;
private:
    glm::vec3 c;
    float rad;
};
#endif
