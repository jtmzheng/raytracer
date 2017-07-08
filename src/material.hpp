#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
#include <memory>

#include "texture.hpp"

using glm::vec3;

class Material {
public:
    Material(const TexturePtr &kd, const TexturePtr &ks, const TexturePtr &km, bool mirror, float p) :
        kd(kd), ks(ks), km(km), mirror(mirror), p(p) { }
public:
    TexturePtr kd;
    TexturePtr ks;
    TexturePtr km;
    bool mirror;
    float p;
};

typedef std::shared_ptr<Material> MaterialPtr;
#endif
