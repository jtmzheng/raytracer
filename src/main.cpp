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
#include "parser.hpp"

#include "tiny_obj_loader.h"
#include "CLI11.hpp"
#include "rapidjson/document.h"

using namespace std;

int main(int ac, char *av[])
{
    // Parse options using CLI11
    string file_name = "./img/test.png", input_file = "test.json";
    uint num_threads = thread::hardware_concurrency();
    CLI::App app("Raytracer");
    app.add_option("-f,--file,file", file_name, "Output file name");
    app.add_option("-t,--thread,thread", num_threads, "Number of threads");
    app.add_option("-i,--input,input", input_file, "Input json file name");

    try {
        app.parse(ac, av);
        cout << "Using " << num_threads << " threads" << endl;
    } catch(const CLI::Error &e) {
        return app.exit(e);
    } catch(...) {
        cerr << "Exception of unknown type!" << endl;
    }

    // Read json for scene
    ifstream is(input_file);
    string file_str;
    is.seekg(0, ios::end);
    file_str.reserve(is.tellg());
    is.seekg(0, ios::beg);
    file_str.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());

    // Parse scene
    rapidjson::Document doc;
    rapidjson::Reader reader;
    doc.Parse(file_str.c_str());

    Image img(1024, 1024);
    RayTracer rt = parseSceneCamera(doc, num_threads);
    auto mtl = parseMaterials(doc);
    auto objs = parseScene(doc, mtl);
    int i = 0;
    for (const auto &obj : objs) {
        rt.addObject(obj, to_string(i++));
    }

    auto lights = parseLights(doc);
    i = 0;
    for (const auto &l : lights) {
        rt.addLight(l, to_string(i++));
    }

    // Timing code
    auto begin = chrono::high_resolution_clock::now();
    rt.render(img);
    auto end = chrono::high_resolution_clock::now();

    cout << "Time elapsed: " << chrono::duration_cast<std::chrono::seconds>(end-begin).count() << " s" << endl;
    img.savePng(file_name);
    cout << "Saved image to " << file_name  << endl;
    return 0;
}
