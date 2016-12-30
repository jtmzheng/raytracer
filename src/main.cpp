#include <glm/vec3.hpp>
#include <iostream>
#include <memory>
#include <boost/program_options.hpp>

#include "image.hpp"
#include "material.hpp"
#include "raytracer.hpp"
#include "surface.hpp"

using namespace std;
namespace po = boost::program_options;

int main(int ac, char *av[])
{
    // Parse options using boost::program_options
    string file_name = "./img/test.png";
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("file,f", po::value<string>(&file_name), "file name")
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
    } catch(exception& e) {
        cerr << "error: " << e.what() << endl;
        return 1;
    } catch(...) {
        cerr << "Exception of unknown type!" << endl;
    }

    Image img(1024, 1024);
    RayTracer rt(glm::vec3(0, 20, 100),
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

    rt.addSurface(shared_ptr<Surface>(
        new Sphere(glm::vec3(0, 30, -50), 25.0f, a)),
        "test"
    );

    rt.addSurface(shared_ptr<Surface>(
        new Sphere(
            glm::vec3(0, -25, -50),
            25.0f,
            b)),
        "test2"
    );

    rt.addSurface(shared_ptr<Surface>(
        new Plane(
            glm::vec3(-25, 0, 0),
            glm::vec3(-25, 5, 0),
            glm::vec3(-25, -5, -5),
            c)),
        "test3"
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

    rt.render(img);
    img.savePng(file_name);
    cout << "Saved image to " << file_name  << endl;
    return 0;
}
