#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

using glm::vec3;

class Material {
public:
    Material(vec3 kd, vec3 ks, bool isMirror, float p) :
        kd(kd), ks(ks), isMirror(isMirror), p(p) { }
public:
    vec3 kd;
    vec3 ks;
    bool isMirror;
    float p;
};

#endif
