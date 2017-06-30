#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
#include <memory>

using glm::vec3;

class Material {
public:
    Material(vec3 kd, vec3 ks, bool isMirror, float p, vec3 km = vec3(0.3, 0.3, 0.3)) :
        kd(kd), ks(ks), isMirror(isMirror), p(p), km(km){ }
public:
    vec3 kd;
    vec3 ks;
    bool isMirror;
    float p;
    vec3 km;
};


typedef std::shared_ptr<Material> MaterialPtr;
#endif
