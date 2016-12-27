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
        glm::vec3(0, 0, 800),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0),
        50);

    rt.addSurface(shared_ptr<Surface>(
        new Sphere(
            glm::vec3(0.7, 1.0, 0.7),
            glm::vec3(0.5, 0.7, 0.5),
            25,
            glm::vec3(45, 45, -100),
            25.0f)),
        "test"
    );
    rt.addLight(shared_ptr<Light>(
        new Light(
            glm::vec3(-100, 150, 400),
            glm::vec3(0.7, 0.0, 0.7))),
        "orange");
    rt.render();
    cout << "Hello World" << endl;
    return 0;
}
