#include "texture.hpp"

#include <cmath>
#include <iostream>
#include <glm/glm.hpp>

using namespace std;

glm::vec3 TextureImage::getCol(const glm::vec2 &uv) const {
    uint i = glm::clamp<uint>(floor(uv.x*img->width()), 0, img->width() - 1);
    uint j = glm::clamp<uint>(floor(uv.y*img->height()), 0, img->height() - 1);

    // Fractional component between pixels
    float up = uv.x*img->width() - floor(uv.x*img->width());
    float vp = uv.y*img->height() - floor(uv.y*img->height());
    glm::vec3 c00 = glm::vec3((*img)(i, j, 0), (*img)(i, j, 1), (*img)(i, j, 2));

    if ((i + 1) >= img->width() || (j + 1) >= img->height()) {
        // Use nearest neighbour for edge
        return c00;
    }
    glm::vec3 c10 = glm::vec3((*img)(i + 1, j, 0), (*img)(i + 1, j, 1), (*img)(i + 1, j, 2));
    glm::vec3 c01 = glm::vec3((*img)(i, j + 1, 0), (*img)(i, j + 1, 1), (*img)(i, j + 1, 2));
    glm::vec3 c11 = glm::vec3((*img)(i + 1, j + 1, 0), (*img)(i + 1, j + 1, 1), (*img)(i + 1, j + 1, 2));

    // Bilinear interpolation
    auto col = (1 - up)*(1 - vp)*c00
        + up*(1 - vp)*c10
        + (1 - up)*vp*c01
        + up*vp*c11;

    // NB: Without clamping odd artifact (clipping?) near sphere pole (vp ~= 0)
    col.x = glm::clamp<float>(col.x, 0, 1);
    col.y = glm::clamp<float>(col.y, 0, 1);
    col.z = glm::clamp<float>(col.z, 0, 1);
    return col;
}
