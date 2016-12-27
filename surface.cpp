#include "surface.hpp"

#include <iostream>

using namespace std;

float det(float a, float b, float c) {
    return b*b - 4*a*c;
}

bool Sphere::intersect(
    const glm::vec3 &eye,
    const glm::vec3 &dir,
    HitRecord &hr) const {
    auto eyeC = eye - c;
    auto a = glm::dot(dir, dir),
         b = glm::dot(2.0f*dir, eyeC),
         c = glm::dot(eyeC, eyeC) - rad*rad;
    auto d = det(a, b, c);

    if (d < 0) {
        return false;
    }

    float t1 = (-b - std::sqrt(d))/(2*a),
          t2 = (-b + std::sqrt(d))/(2*a);
    if (t1 > 0) {
        hr.t = t1;
        return true;
    }
    if (t2 > 0) {
        hr.t = t2;
        return true;
    }
    return false;
}

glm::vec3 Sphere::getNorm(const glm::vec3 &pos) const {
    return glm::normalize(pos - c);
}
