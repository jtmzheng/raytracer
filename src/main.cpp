#include <glm/vec3.hpp>
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

#include "image.hpp"
#include "material.hpp"
#include "raytracer.hpp"
#include "surface.hpp"
#include "objobject.hpp"
#include "transform.hpp"

#include "tiny_obj_loader.h"
#include "CLI11.hpp"

using namespace std;

int main(int ac, char *av[])
{
    // Parse options using CLI11
    string file_name = "./img/test.png";
    uint num_threads = thread::hardware_concurrency();
    CLI::App app("Raytracer");
    app.add_option("-f,--file,file", file_name, "Output file name");
    app.add_option("-t,--thread,thread", num_threads, "Number of threads");

    try {
        app.parse(ac, av);
        cout << "Using " << num_threads << " threads" << endl;
    } catch(const CLI::Error &e) {
        return app.exit(e);
    } catch(...) {
        cerr << "Exception of unknown type!" << endl;
    }

    Image img(1024, 1024);
    RayTracer rt(num_threads,
        glm::vec3(0, 20, 100),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0),
        50);

    auto a = make_shared<Material>(
        glm::vec3(0.7, 1.0, 0.7),
        glm::vec3(0.3, 0.3, 0.3),
        true,
        10);

    auto b = make_shared<Material>(
        glm::vec3(0.7, 0.1, 0.3),
        glm::vec3(0.3, 0.3, 0.3),
        true,
        10);

    auto c = make_shared<Material>(
        glm::vec3(0.9, 0.9, 0.9),
        glm::vec3(0.9, 0.9, 0.9),
        true,
        100);

    rt.addObject(shared_ptr<Surface>(
        new Sphere(glm::vec3(0, 30, -50), 25.0f, a)),
        "test"
    );

    rt.addObject(shared_ptr<Surface>(
        new Sphere(
            glm::vec3(0, -25, -50),
            25.0f,
            b)),
        "test2"
    );

    auto rot = TransformMatrix::rot(glm::vec3(0, 0, 1), 90.0f);
    auto scale = TransformMatrix::scale(glm::vec3(25, 25, 25));
    auto transform = TransformChain({rot, scale});
    auto objs = ObjObject::loadFromFile("../obj/teapot.obj", "../obj/", transform);
    int i = 0;
    for (const auto &obj : objs) {
        if(!rt.addObject(obj, to_string(i++))) {
            cout << "error" << endl;
        }
        cout << "adding object" << endl;
    }

    rt.addObject(Plane::create(
        glm::vec3(-25, 0, 0),
        glm::vec3(-25, 5, 0),
        glm::vec3(-25, -5, -5),
        c),
        "plane"
    );

    rt.addLight(shared_ptr<Light>(
        new Light(
            glm::vec3(25, 150, 0),
            glm::vec3(0.7, 0.7, 0.7),
            10.0f)),
        "white");

    rt.addLight(shared_ptr<Light>(
        new Light(
            glm::vec3(0, 20, 50),
            glm::vec3(0.7, 0.7, 0.7),
            10.0f)),
        "white2");

    // Timing code
    auto begin = chrono::high_resolution_clock::now();
    rt.render(img);
    auto end = chrono::high_resolution_clock::now();

    cout << "Time elapsed: " << chrono::duration_cast<std::chrono::seconds>(end-begin).count() << " s" << endl;
    img.savePng(file_name);
    cout << "Saved image to " << file_name  << endl;
    return 0;
}
