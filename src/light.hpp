#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light {
public:
    Light(glm::vec3 pos, glm::vec3 intensity) :
        intensity(intensity), pos(pos) { }

    glm::vec3 getIntensity() const { return intensity; }
    glm::vec3 getPos() const { return pos; }
private:
    glm::vec3 intensity;
    glm::vec3 pos;
};

#endif
