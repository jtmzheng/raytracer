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

            HitRecord minHr{std::numeric_limits<float>::max(), glm::vec3(), glm::vec3(), nullptr};
            for (const auto &surf : surfs) {
                HitRecord hr{std::numeric_limits<float>::max(), dir, eye, surf};
                if (surf->intersect(eye, dir, hr)) {
                    if (hr.t < minHr.t) {
                        minHr = hr;
                    }
                }
            }

            // No intersection found
            if (minHr.surf == nullptr)
                continue;

            auto int_pt = eye + dir*minHr.t;
            auto v = -dir, norm = minHr.surf->getNorm(int_pt);
            glm::vec3 col(0, 0, 0);
            for (const auto &light : lights) {
                auto lightDir = glm::normalize(light->getPos() - int_pt);
                auto halfVec = glm::normalize(lightDir + v);

                // Phong lighting
                auto kd = minHr.surf->getKd(), ks = minHr.surf->getKs();
                auto p = minHr.surf->getP();
                auto intensity = light->getIntensity();
                auto diffMag = max(0.0f, glm::dot(norm, lightDir));
                auto specMag = glm::pow(glm::max(0.0f, glm::dot(norm, halfVec)), p);

                col.r += kd.r*intensity.r*diffMag + ks.r*intensity.r*specMag;
                col.g += kd.g*intensity.g*diffMag + ks.g*intensity.g*specMag;
                col.b += kd.b*intensity.b*diffMag + ks.b*intensity.b*specMag;
            }

            img(i, j, 0) = col.r;
            img(i, j, 1) = col.g;
            img(i, j, 2) = col.b;
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

bool RayTracer::addLight(shared_ptr<Light> light, string name) {
    if (!lightNames.count(name)) {
        lightNames[name] = lights.size();
        lights.emplace_back(light);
        return true;
    }

    return false;
}
