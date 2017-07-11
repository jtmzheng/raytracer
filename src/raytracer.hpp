#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <atomic>
#include <condition_variable>

#include "image.hpp"
#include "light.hpp"
#include "surface.hpp"

using namespace std;

const static float DEFAULT_ASPECT_RATIO = 1.0f;
const static float MAX_DEPTH = 10;

struct JobData;

class RayTracer {
public:
    RayTracer(uint numThreads,
        glm::vec3 eye,
        glm::vec3 lookAt,
        glm::vec3 up,
        float fovy) :
            numThreads(numThreads),
            eye(eye),
            lookAt(lookAt),
            u(glm::normalize(up)),
            fovy(fovy)
    {
        w = eye - lookAt;
        focalLength = glm::length(w);
        w = glm::normalize(w);
        v = glm::cross(w, u);

        // Compute the image dimensions
        impH = glm::tan(glm::radians(this->fovy/2)) * 2 * focalLength;
        impW = impH * DEFAULT_ASPECT_RATIO; // Override with render aspect ratio
    }

    void render(Image &img);
    bool addObject(shared_ptr<Object> obj, string name);
    bool addLight(shared_ptr<Light> l, string name);

private:
    bool intersect(
        const glm::vec3 &eye,
        const glm::vec3 &dir,
        HitRecord &hr,
        pair<float, float> rng = make_pair(0, numeric_limits<float>::max())) const;

    // Worker job to return color per pixel
    void traceRows(const glm::vec3 &l, JobData &job, atomic<uint> &jobsLeft, Image &img, condition_variable &lbCv) const;
    void tracePixel(const glm::vec3 &l, Image &img, uint i, uint j) const;
    void loadBalance(vector<JobData> &jobs, atomic<uint> &jobsLeft, mutex &lbMutex, condition_variable &lbCv) const;

    // Shade a pixel using the HitRecord for the ray through that pixel
    glm::vec3 shade(const glm::vec3 &eye, const HitRecord &hr, int depth = 0) const;
    glm::vec3 raycolor(const glm::vec3 &eye, const glm::vec3 &dir, int depth = 0) const;

private:
    uint numThreads;

    // View parameters
    glm::vec3 eye;
    glm::vec3 lookAt;
    glm::vec3 w, u, v;
    float fovy, focalLength;

    // Image plane dimensions
    float impH;
    float impW;

    // Light parameters
    vector<shared_ptr<Light>> lights;
    map<string, int> lightNames;

    // Objects in world
    vector<shared_ptr<Object>> objs;
    map<string, int> objNames;
};
#endif
