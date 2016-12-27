#include "raytracer.hpp"
#include "surface.hpp"

#include <iostream>
#include <glm/ext.hpp>

void RayTracer::render() {
    // NB: 'c' is centre of image plane, 'l' is lower left hand corner
    auto c = eye - w*focalLength;
    auto l = c - u*impW/2.0f - v*impH/2.0f;

    std::cout << glm::to_string(eye) << endl;
    std::cout << focalLength << endl;
    std::cout << glm::to_string(c) << std::endl;
    std::cout << glm::to_string(l) << std::endl;
    std::cout << "Begin" << std::endl;
    std::cout << img.width() << ", " << img.height() << std::endl;

    for (uint i = 0; i < img.width(); ++i) {
        for (uint j = 0; j < img.height(); ++j) {
            // Pixel location on image plane
            auto us = (i + 0.5f)*impW/float(img.width());
            auto vs = (j + 0.5f)*impH/float(img.height());
            auto s = l + u*us + v*vs;
            auto dir = glm::normalize(s - eye);
            //std::cout << i << ", " << j << std::endl;
            //std::cout << glm::to_string(dir) << std::endl;

            HitRecord minHr{std::numeric_limits<float>::max(), glm::vec3(), glm::vec3(), nullptr};
            for (const auto &surf : surfs) {
                HitRecord hr{std::numeric_limits<float>::max(), dir, eye, surf};
                if (surf->intersect(eye, dir, hr)) {
                    std::cout << "Hit " << hr.t << std::endl;
                    img(i, j, 0) = 1;
                    if (hr.t < minHr.t) {
                        minHr = hr;
                    }
                }
            }

        }
    }


    img.savePng("test.png");
}

bool RayTracer::addSurface(shared_ptr<Surface> surf, string name) {
    if (!surfNames.count(name)) {
        surfNames[name] = surfs.size();
        surfs.emplace_back(surf);
        return true;
    }

    return false;
}
