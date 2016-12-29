#include "raytracer.hpp"
#include "surface.hpp"

#include <iostream>
#include <glm/ext.hpp>
#include <random>

static const float EPS = 1e-3;
static const float SUBSAMPLES = 4;
static const float SUBSAMPLE_DIV = 1.0f / SUBSAMPLES;

void RayTracer::render(Image &img) {
    // NB: 'c' is centre of image plane, 'l' is lower left hand corner
    auto c = eye - w*focalLength;
    auto l = c - u*impW/2.0f - v*impH/2.0f;

    // NB: See http://stackoverflow.com/questions/686353/c-random-float-number-generation
    ::random_device rd;
    ::mt19937 e2(rd());
    ::uniform_real_distribution<float> dist(0, SUBSAMPLE_DIV);

    for (uint i = 0; i < img.width(); ++i) {
        for (uint j = 0; j < img.height(); ++j) {
            // Stratified supersampling
            glm::vec3 col(0, 0, 0);
            for (float p = 0; p < SUBSAMPLES; ++p) {
                for (float q = 0; q < SUBSAMPLES; ++q) {
                    // Pixel location on image plane
                    auto us = (i + (p + dist(e2)/SUBSAMPLES))*impW/float(img.width());
                    auto vs = (j + (q + dist(e2)/SUBSAMPLES))*impH/float(img.height());
                    auto s = l + u*us + v*vs;
                    auto dir = glm::normalize(s - eye);

                    // Compute the color for the ray
                    col += raycolor(eye, dir);
                }
            }
            col /= float(SUBSAMPLES*SUBSAMPLES);
            img(i, j, 0) = col.r;
            img(i, j, 1) = col.g;
            img(i, j, 2) = col.b;
        }
    }
}

glm::vec3 RayTracer::raycolor(const glm::vec3 &eye, const glm::vec3 &dir, int depth) const {
    // Find surface with minimum intersection for ray
    HitRecord minHr{std::numeric_limits<float>::max(), glm::vec3(), glm::vec3(), nullptr};
    if (!intersect(eye, dir, minHr)) {
        // TODO: Add background
        return glm::vec3();
    }

    return shade(minHr, depth);

}

bool RayTracer::intersect(
    const glm::vec3 &eye,
    const glm::vec3 &dir,
    HitRecord &minHr,
    std::pair<float, float> rng) const
{
    for (const auto &surf : surfs) {
        HitRecord hr{std::numeric_limits<float>::max(), dir, eye, surf};
        if (surf->intersect(eye, dir, hr, rng)) {
            if (hr.t < minHr.t) {
                minHr = hr;
            }
        }
    }

    return minHr.surf != nullptr;
}

glm::vec3 RayTracer::shade(const HitRecord &hr, int depth) const {
    auto int_pt = eye + hr.dir*hr.t;
    auto v = -hr.dir, norm = hr.surf->getNorm(int_pt);
    // Surface params
    const auto &mat = hr.surf->getMaterial();

    glm::vec3 col(0, 0, 0);
    for (const auto &light : lights) {
        auto lightDir = light->getPos() - int_pt;
        auto lightDist = glm::length(lightDir);
        lightDir = glm::normalize(lightDir);
        auto halfVec = glm::normalize(lightDir + v);

        // Check if in shadow
        HitRecord shadHr{std::numeric_limits<float>::max(), glm::vec3(), glm::vec3(), nullptr};
        if (intersect(int_pt + EPS*lightDir, lightDir, shadHr, make_pair(0, lightDist))) {
            // Don't shade if in shadow of another surface
            continue;
        }

        auto intensity = light->getIntensity();
        auto diffMag = max(0.0f, glm::dot(norm, lightDir));
        float specMag = 0;

        // Ignore Phong specular shading if mirrored surface
        if (!mat.isMirror) {
            specMag = glm::pow(glm::max(0.0f, glm::dot(norm, halfVec)), mat.p);
        }

        col.r += mat.kd.r*intensity.r*diffMag + mat.ks.r*intensity.r*specMag;
        col.g += mat.kd.g*intensity.g*diffMag + mat.ks.g*intensity.g*specMag;
        col.b += mat.kd.b*intensity.b*diffMag + mat.ks.b*intensity.b*specMag;
    }

    // Mirror reflections
    if (mat.isMirror && depth < MAX_DEPTH) {
        // NB: ks should probably be km here
        auto r = glm::normalize(hr.dir - 2*(glm::dot(hr.dir, norm))*norm);
        col += mat.ks * raycolor(int_pt + EPS*r, r, depth + 1);
    }

    return col;
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