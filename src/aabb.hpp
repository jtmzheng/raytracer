#ifndef AABB_H
#define AABB_H

#include <climits>
#include <algorithm>
#include <glm/glm.hpp>

// Axis Aligned Bounding Box
struct AABB {
    AABB() {
        x = std::make_pair(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());
        y = std::make_pair(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());
        z = std::make_pair(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());
    }

    void update(const glm::vec3 &p) {
        x.first = std::min(x.first, p.x);
        y.first = std::min(y.first, p.y);
        z.first = std::min(z.first, p.z);
        x.second = std::max(x.second, p.x);
        y.second = std::max(y.second, p.y);
        z.second = std::max(z.second, p.z);
    }

    std::pair<float, float> x;
    std::pair<float, float> y;
    std::pair<float, float> z;
};

// Slab intersection (probably need better place for this)
static inline std::pair<float, float> calcT(float xe, float xd, const std::pair<float, float> &rng) {
    float a = 1.0f/xd;
    auto pr = std::make_pair(0.0f, 0.0f);
    if (a >= 0) {
        pr.first = a*(rng.first - xe);
        pr.second = a*(rng.second - xe);
    } else {
        pr.second = a*(rng.first - xe);
        pr.first = a*(rng.second - xe);
    }
    return pr;
}
#endif
