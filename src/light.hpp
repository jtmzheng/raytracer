#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light {
public:
    Light(glm::vec3 pos, glm::vec3 intensity, float rad) :
        intensity(intensity), pos(pos), rad(rad) { }

    // Returns a point in the spherical light
    // TODO: Sample uniformly using spherical coordinates (this is pseudo-uniform)
    glm::vec3 sample(float r1, float r2, float r3) const;
public:
    const glm::vec3 intensity;
    const glm::vec3 pos;
    const float rad;
};

#endif
