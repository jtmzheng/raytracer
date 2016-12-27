#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "image.hpp"
#include "surface.hpp"

using namespace std;

const float ASPECT_RATIO = 1.0f;

class RayTracer {
public:
    RayTracer(Image img,
        glm::vec3 eye,
        glm::vec3 lookAt,
        glm::vec3 up,
        float fovy) :
            img(img),
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
        impH = glm::tan(glm::radians(fovy/2)) * 2 * focalLength;
        impW = impH * ASPECT_RATIO;
        cout << impH << ", " << impW << endl;
    }

    void render();
    bool addSurface(shared_ptr<Surface> surf, string name);
    bool addLight(string name);

private:
    // Output image
    Image img;

    // View parameters
    glm::vec3 eye;
    glm::vec3 lookAt;
    glm::vec3 w, u, v;
    float fovy, focalLength;

    // Image plane dimensions
    float impH;
    float impW;

    // Light parameters
    glm::vec3 ambient;
    // TODO: Light class

    // Objects in world
    vector<shared_ptr<Surface>> surfs;
    map<string, int> surfNames;
};
#endif
