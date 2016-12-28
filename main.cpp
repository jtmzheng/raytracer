#include <glm/vec3.hpp>
#include <iostream>
#include <memory>

#include "image.hpp"
#include "raytracer.hpp"
#include "surface.hpp"

using namespace std;

int main()
{
    Image img(512, 512);
    RayTracer rt(img,
        glm::vec3(0, 20, 100),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0),
        50);

    rt.addSurface(shared_ptr<Surface>(
        new Sphere(
            glm::vec3(0.7, 1.0, 0.7),
            glm::vec3(0.5, 0.7, 0.5),
            25,
            glm::vec3(0, 30, -50),
            25.0f)),
        "test"
    );

    rt.addSurface(shared_ptr<Surface>(
        new Sphere(
            glm::vec3(0.7, 0.1, 0.3),
            glm::vec3(0.5, 0.1, 0.3),
            25,
            glm::vec3(0, -25, -50),
            25.0f)),
        "test2"
    );

    rt.addSurface(shared_ptr<Surface>(
        new Plane(
            glm::vec3(-25, 0, 0),
            glm::vec3(-25, 5, 0),
            glm::vec3(-25, -5, -5))),
        "test3"
    );

    rt.addLight(shared_ptr<Light>(
        new Light(
            glm::vec3(25, 150, 0),
            glm::vec3(0.7, 0.7, 0.7))),
        "white");

    rt.addLight(shared_ptr<Light>(
        new Light(
            glm::vec3(0, 20, 50),
            glm::vec3(0.7, 0.7, 0.7))),
        "white2");

    rt.render();
    cout << "Hello World" << endl;
    return 0;
}
