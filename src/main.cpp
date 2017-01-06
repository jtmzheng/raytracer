#include <glm/vec3.hpp>
#include <iostream>
#include <memory>
#include <boost/program_options.hpp>
#include <chrono>
#include <thread>

#include "image.hpp"
#include "material.hpp"
#include "raytracer.hpp"
#include "surface.hpp"
#include "objobject.hpp"
#include "transform.hpp"

#include "tiny_obj_loader.h"

using namespace std;
namespace po = boost::program_options;

int main(int ac, char *av[])
{
    // Parse options using boost::program_options
    string file_name = "./img/test.png";
    uint num_threads = thread::hardware_concurrency();
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("file,f", po::value<string>(&file_name), "file name")
            ("threads,t", po::value<uint>(&num_threads), "number of threads")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);

        if (vm.count("file")) {
            cout << "Outputing image to "
                 << vm["file"].as<string>() << endl;
        } else {
            cout << "Outputing image to ./img/test.png" << endl;
        }

        cout << "Using " << num_threads << " threads" << endl;
    } catch(exception& e) {
        cerr << "error: " << e.what() << endl;
        return 1;
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
