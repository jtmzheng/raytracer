#include "texture.hpp"

#include "math.h"

using namespace std;

glm::vec3 TextureImage::getCol(const glm::vec2 &uv) const {
    // TODO: Bilinear interp
    int i = floor(uv.x * img->width());
    int j = floor(uv.y * img->height());
    return glm::vec3((*img)(i, j, 0), (*img)(i, j, 1), (*img)(i, j, 2));
}
