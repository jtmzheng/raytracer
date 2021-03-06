#include "surface.hpp"

#include <iostream>
#include <cmath>
#include <iostream>

using namespace std;

#define EPS 1e-6f

float discriminant(float a, float b, float c) {
    return b*b - 4*a*c;
}

bool Sphere::intersect(
    const glm::vec3 &eye,
    const glm::vec3 &dir,
    HitRecord &hr,
    const std::pair<float, float> &rng) const {
    auto eyeC = eye - c;
    auto a = glm::dot(dir, dir),
         b = glm::dot(2.0f*dir, eyeC),
         c = glm::dot(eyeC, eyeC) - rad*rad;
    auto d = discriminant(a, b, c);

    if (d < 0) {
        return false;
    }

    float t1 = (-b - std::sqrt(d))/(2*a),
          t2 = (-b + std::sqrt(d))/(2*a);

    if (t1 > rng.first && t1 < rng.second && t1 < hr.t) {
        hr.t = t1;
        hr.surf = this;
        hr.norm = getNorm(eye + hr.t*dir);
        hr.uv = getUV(eye + hr.t*dir);
        return true;
    }
    if (t2 > rng.first && t2 < rng.second && t2 < hr.t) {
        hr.t = t2;
        hr.surf = this;
        hr.norm = getNorm(eye + hr.t*dir);
        hr.uv = getUV(eye + hr.t*dir);
        return true;
    }
    return false;
}

glm::vec3 Sphere::getNorm(const glm::vec3 &pos) const {
    return glm::normalize(pos - c);
}

glm::vec2 Sphere::getUV(const glm::vec3 &pos) const {
    // Assume sphere poles aligned with z-axis
    float u = 0.5 + atan2(pos.y - c.y, pos.x - c.x) / (2 * M_PI);
    float v = acos((pos.z - c.z) / rad) / M_PI;
    return glm::vec2(u, v);
}
