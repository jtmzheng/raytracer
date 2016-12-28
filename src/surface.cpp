#include "surface.hpp"

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
    if (t1 > rng.first && t1 < rng.second) {
        hr.t = t1;
        return true;
    }
    if (t2 > rng.first && t2 < rng.second) {
        hr.t = t2;
        return true;
    }
    return false;
}

glm::vec3 Sphere::getNorm(const glm::vec3 &pos) const {
    return glm::normalize(pos - c);
}

bool Triangle::intersect(
    const glm::vec3 &eye,
    const glm::vec3 &dir,
    HitRecord &hr,
    const std::pair<float, float> &rng) const {
    // NB: ea is not normalized on purpose!
    auto ea = va - eye;

    // NB: these variable names are for ease of matching eqn (Cramers) from source material
    float a = ba.x, b = ba.y, c = ba.z,
          d = ca.x, e = ca.y, f = ca.z,
          g = dir.x, h = dir.y, i = dir.z;
    float j = ea.x, k = ea.y, l = ea.z;

    float M = a*(e*i-h*f) + b*(g*f-d*i) + c*(d*h-e*g);
    if (abs(M) < EPS)
        return false;

    float t = -(f*(a*k-j*b) + e*(j*c-a*l) + d*(b*l-k*c)) / M;
    if (t < rng.first || t > rng.second)
        return false;

    float gamma = (i*(a*k-j*b) + h*(j*c-a*l) + g*(b*l-k*c)) / M;
    if (isFinite() && (gamma < 0 || gamma > 1))
        return false;

    float beta = (j*(e*i-h*f) + k*(g*f-d*i) + l*(d*h-e*g)) / M;
    if (isFinite() && (beta < 0 || beta > (1 - gamma)))
        return false;

    hr.t = t;
    return true;
}

glm::vec3 Triangle::getNorm(const glm::vec3 &pos) const {
    // NB: Not sure which normal is correct
    return glm::normalize(glm::cross(ca, ba));
}
