#include "raytracer.hpp"
#include "surface.hpp"

#include <iostream>
#include <glm/ext.hpp>
#include <random>

#include <thread>
#include <mutex>
#include <algorithm>
#include <queue>
#include <chrono>

static const float EPS = 1e-3;
static const float SUBSAMPLES = 4;
static const float SUBSAMPLE_DIV = 1.0f / SUBSAMPLES;
static const float SHADOWSAMPLES = 16;

// Per job specific data
struct JobData {
    queue<pair<uint, uint>> jobsQueue;
    mutex queueMutex;
};

void RayTracer::render(Image &img) {
    // NB: 'c' is centre of image plane, 'l' is lower left hand corner
    impW = impH * (float(img.width()) / float(img.height()));
    auto c = eye - w*focalLength;
    auto l = c - u*impW/2.0f - v*impH/2.0f;
    auto chunkSize = (img.height() / numThreads) + (img.height() % numThreads != 0);
    vector<thread> workers(numThreads);
    vector<JobData> jobs(numThreads);
    atomic<uint> jobsLeft(img.width() * img.height());
    mutex lbMutex;
    condition_variable lbCv;

    for (uint chunk = 0; chunk < numThreads; ++chunk) {
        for (uint j = chunk*chunkSize; j < min((chunk+1)*chunkSize, img.height()); ++j) {
            for (uint i = 0; i < img.width(); ++i) {
                jobs[chunk].jobsQueue.emplace(make_pair(i, j));
            }
        }

        // NB: We can bind a reference to thread since thread lifetime < lifetime of this function
        workers[chunk] = thread(
            &RayTracer::traceRows, this, ref(l), ref(jobs[chunk]), ref(jobsLeft), ref(img), ref(lbCv)
        );
    }

    auto lbThread = thread(&RayTracer::loadBalance, this, ref(jobs), ref(jobsLeft), ref(lbMutex), ref(lbCv));
    cout << "Waiting for threads to join..." << endl;
    std::for_each(workers.begin(), workers.end(), [](thread& x){ x.join(); });
    lbThread.join();
}

void RayTracer::traceRows(const glm::vec3 &l, JobData &job, atomic<uint> &jobsLeft, Image &img, condition_variable &lbCv) const {
    while (jobsLeft > 0) {
        job.queueMutex.lock();
        if (job.jobsQueue.empty()) {
            lbCv.notify_one();
            job.queueMutex.unlock();

            // Sleep for half second to reduce busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } else {
            auto i = job.jobsQueue.front().first;
            auto j = job.jobsQueue.front().second;
            job.jobsQueue.pop();
            job.queueMutex.unlock();
            jobsLeft--;
            tracePixel(l, img, i, j);
        }
    }
    cout << "Done job" << endl;
}

void RayTracer::loadBalance(vector<JobData> &jobs, atomic<uint> &jobsLeft, mutex &lbMutex, condition_variable &lbCv) const {
    unique_lock<mutex> l(lbMutex);
    while (jobsLeft > 0) {
        // NB: This may be missed, but still correct (want to avoid signals immediately after lb finishes
        lbCv.wait_for(l, std::chrono::seconds(10));
        if (jobsLeft < 100*jobs.size()) {
            // Stop load balancing when there are not enough jobs to be worthwhile
            cout << "Stopping load balancing... " << jobsLeft << " jobs left" << endl;
            break;
        }

        for (auto &job : jobs) {
            job.queueMutex.lock();
        }

        // Rebalance all the queues (pseudo-uniform [sic])
        vector<queue<pair<uint, uint>>> aux(jobs.size());
        queue<pair<uint, uint>> src;
        uint perQueue = jobsLeft / jobs.size();
        for (auto &job : jobs) {
            while (job.jobsQueue.size() > perQueue) {
                src.emplace(job.jobsQueue.front());
                job.jobsQueue.pop();
            }
        }

        // Get each queue up to perQueue
        for (auto &job : jobs) {
            while (job.jobsQueue.size() < perQueue && !src.empty()) {
                job.jobsQueue.emplace(src.front());
                src.pop();
            }
        }

        // Add unbalanced jobs in round robin
        uint i = 0;
        while (!src.empty()) {
            jobs[i % jobs.size()].jobsQueue.emplace(src.front());
            src.pop();
            i++;
        }

        cout << "Queue Sizes: ";
        for (uint i = 0; i < jobs.size(); ++i) {
            auto &job = jobs[i];
            cout << job.jobsQueue.size() << " ";
            job.queueMutex.unlock();
        }
        cout << endl;
    }
}

void RayTracer::tracePixel(const glm::vec3 &l, Image &img, uint i, uint j) const {
    // NB: See http://stackoverflow.com/questions/686353/c-random-float-number-generation
    thread_local random_device rd;
    thread_local mt19937 e2(rd());
    thread_local uniform_real_distribution<float> dist(0, SUBSAMPLE_DIV);

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

glm::vec3 RayTracer::raycolor(const glm::vec3 &eye, const glm::vec3 &dir, int depth) const {
    // Find surface with minimum intersection for ray
    HitRecord minHr{std::numeric_limits<float>::max(), eye, dir, nullptr, glm::vec3(), glm::vec2()};
    if (!intersect(eye, dir, minHr)) {
        // TODO: Add background if no intersection found
        return glm::vec3();
    }

    return shade(eye, minHr, depth);

}

bool RayTracer::intersect(
    const glm::vec3 &eye,
    const glm::vec3 &dir,
    HitRecord &minHr,
    std::pair<float, float> rng) const
{
    for (const auto &obj: objs) {
        HitRecord hr{std::numeric_limits<float>::max(), dir, eye, nullptr, glm::vec3()};
        if (obj->intersect(eye, dir, hr, rng)) {
            if (hr.t < minHr.t) {
                minHr = hr;
                rng.second = min(rng.second, hr.t);
            }
        }
    }

    return minHr.surf != nullptr;
}

glm::vec3 RayTracer::shade(const glm::vec3 &eye, const HitRecord &hr, int depth) const {
    auto int_pt = eye + hr.dir*hr.t;
    auto v = -hr.dir;
    // Surface params
    const auto &mat = hr.surf->getMaterial();

    // Local random device since rng is not threadsafe
    thread_local random_device rd;
    thread_local mt19937 e2(rd());
    thread_local uniform_real_distribution<float> dist(0, 1);

    glm::vec3 col(0, 0, 0);

    // Add color contribution for each light
    for (const auto &light : lights) {
        glm::vec3 lightCol(0, 0, 0);
        for (uint i = 0; i < SHADOWSAMPLES; ++i) {
            auto lightDir = light->sample(dist(e2), dist(e2), dist(e2)) - int_pt;
            auto lightDist = glm::length(lightDir);
            lightDir = glm::normalize(lightDir);
            auto halfVec = glm::normalize(lightDir + v);

            // Check if in shadow by sending shadow ray to light source
            HitRecord shadHr{std::numeric_limits<float>::max(), int_pt + EPS*lightDir, lightDir, nullptr, glm::vec3(), glm::vec2()};
            if (intersect(int_pt + EPS*lightDir, lightDir, shadHr, make_pair(0, lightDist + 2*light->rad))) {
                // Don't shade if in shadow of another surface
                continue;
            }

            // TODO: Attenuate light based on distance?
            auto intensity = light->intensity;
            auto diffMag = max(0.0f, glm::dot(hr.norm, lightDir));
            float specMag = 0;

            // Phong shading (specular + diffuse)
            auto kd = mat.kd->getCol(hr.uv), ks = mat.ks->getCol(hr.uv);
            specMag = glm::pow(glm::max(0.0f, glm::dot(hr.norm, halfVec)), mat.p);
            lightCol.r += kd.r*intensity.r*diffMag + ks.r*intensity.r*specMag;
            lightCol.g += kd.g*intensity.g*diffMag + ks.g*intensity.g*specMag;
            lightCol.b += kd.b*intensity.b*diffMag + ks.b*intensity.b*specMag;
        }

        lightCol /= SHADOWSAMPLES;
        col += lightCol;
    }

    // Calculate mirror reflections recursively
    if (mat.mirror && depth < MAX_DEPTH) {
        auto r = glm::normalize(hr.dir - 2*(glm::dot(hr.dir, hr.norm))*hr.norm);
        auto km = mat.km->getCol(hr.uv);
        col += km * raycolor(int_pt + EPS*r, r, depth + 1);
    }

    return col;
}

bool RayTracer::addObject(shared_ptr<Object> obj, string name) {
    if (!objNames.count(name)) {
        objNames[name] = objs.size();
        objs.emplace_back(obj);
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
