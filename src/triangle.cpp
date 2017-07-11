#include "triangle.hpp"

#include <iostream>

#define EPS 1e-6f

shared_ptr<Plane> Plane::create(glm::vec3 a, glm::vec3 b, glm::vec3 c, MaterialPtr mtl) {
    auto ba = b - a, ca = c - a;
    auto n = glm::normalize(glm::cross(ca, ba));
    return std::make_shared<Plane>(
        Vertex(a, n, glm::vec2()),
        Vertex(b, n, glm::vec2()),
        Vertex(c, n, glm::vec2()),
        mtl
    );
}

bool Triangle::intersect(
    const glm::vec3 &eye,
    const glm::vec3 &dir,
    HitRecord &hr,
    const std::pair<float, float> &rng) const {
    // NB: ea is not normalized on purpose!
    auto ea = va.p - eye;

    // NB: these variable names are for ease of matching eqn (Cramers) from source material
    float a = ba.x, b = ba.y, c = ba.z,
          d = ca.x, e = ca.y, f = ca.z,
          g = dir.x, h = dir.y, i = dir.z;
    float j = ea.x, k = ea.y, l = ea.z;

    float M = a*(e*i-h*f) + b*(g*f-d*i) + c*(d*h-e*g);
    if (glm::abs(M) < EPS)
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

    // Assign t iff smaller than current t (intersect closer)
    if (t < hr.t) {
        hr.t = t;
        hr.surf = this;
        hr.norm = getNorm(glm::vec3(1 - gamma - beta, beta, gamma));
        hr.uv = getUV(glm::vec3(1 - gamma - beta, beta, gamma));
        return true;
    }
    return false;
}

glm::vec3 Triangle::getNorm(const glm::vec3 &bary) const {
    return va.n*bary.x + vb.n*bary.y + vc.n*bary.z;
}

glm::vec2 Triangle::getUV(const glm::vec3 &bary) const {
    return va.uv*bary.x + vb.uv*bary.y + vc.uv*bary.z;
}

glm::vec3 Plane::getNorm(const glm::vec3 &pos) const {
    // All normals on plane are equivalent
    return va.n;
}
