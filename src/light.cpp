#include "light.hpp"

glm::vec3 Light::sample(float r1, float r2, float r3) const {
    return pos + glm::normalize(glm::vec3(r1, r2, r3))*rad;
}
