#include "transform.hpp"

#include <iostream>
#include <glm/ext.hpp>

TransformPtr TransformMatrix::rot(const glm::vec3 &axis, float deg) {
    // From araghavan3 (See GitHub)
    glm::mat3 mat;
    float ct = static_cast<float>(cos(M_PI * deg / 180.0));
    float st = static_cast<float>(sin(M_PI * deg / 180.0));

    float t = 1 - ct;
    float tx = t * axis.x;
    float ty = t * axis.y;
    float tz = t * axis.z;
    float txy = tx * axis.y;
    float txz = tx * axis.z;
    float tyz = ty * axis.z;
    float sx = st * axis.x;
    float sy = st * axis.y;
    float sz = st * axis.z;

    mat[0][0] = tx * axis.x + ct;
    mat[0][1] = txy + sz;
    mat[0][2] = txz - sy;

    mat[1][0] = txy - sz;
    mat[1][1] = ty * axis.y + ct;
    mat[1][2] = tyz + sx;

    mat[2][0] = txz + sy;
    mat[2][1] = tyz - sx;
    mat[2][2] = tz * axis.z + ct;
    return std::make_shared<TransformMatrix>(mat);
}

TransformPtr TransformMatrix::scale(const glm::vec3 &scale) {
    glm::mat3 mat;
    mat[0][0] *= scale.x;
    mat[1][1] *= scale.y;
    mat[2][2] *= scale.z;
    return std::make_shared<TransformMatrix>(mat);
}

TransformPtr TransformTrans::translate(const glm::vec3 &s) {
    return std::make_shared<TransformTrans>(s);
}


glm::vec3 TransformMatrix::pos(const glm::vec3 &pos) const {
    return M * pos;
}

glm::vec3 TransformMatrix::norm(const glm::vec3 &norm) const {
    return N * norm;
}

glm::vec3 TransformChain::pos(const glm::vec3 &pos) const {
    auto curr = pos;
    for (const auto &t : xforms) {
        curr = t->pos(curr);
    }
    return curr;
}

glm::vec3 TransformChain::norm(const glm::vec3 &norm) const {
    auto curr = norm;
    for (const auto &t : xforms) {
        curr = t->norm(curr);
    }
    return curr;
}
