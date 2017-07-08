#ifndef TEXTURE_H
#define TEXTURE_H

#include <glm/glm.hpp>
#include <memory>

#include "image.hpp"

class Texture {
public:
    virtual glm::vec3 getCol(const glm::vec2 &uv) const = 0;
};

class TextureImage : public Texture {
public:
    TextureImage(std::shared_ptr<Image> img) : img(img) { }
    glm::vec3 getCol(const glm::vec2 &uv) const;
private:
    std::shared_ptr<Image> img;
};

class TextureConst : public Texture {
public:
    TextureConst(glm::vec3 col) : col(col) { }
    glm::vec3 getCol(const glm::vec2 &uv) const { return col; }
private:
    glm::vec3 col;
};

typedef std::shared_ptr<Texture> TexturePtr;
#endif
