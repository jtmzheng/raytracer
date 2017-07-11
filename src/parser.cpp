#include <vector>
#include <map>
#include <memory>

#include "rapidjson/document.h"

#include "parser.hpp"
#include "surface.hpp"
#include "raytracer.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "surface.hpp"
#include "objobject.hpp"
#include "transform.hpp"

using namespace std;

// Read image config
pair<int, int> parseImageDim(const rapidjson::Document &doc) {
    const auto &scene = doc["scene"];
    return make_pair(scene["width"].GetInt(), scene["height"].GetInt());
}

// Initialize raytracer camera using json config
RayTracer parseSceneCamera(const rapidjson::Document &doc, uint num_threads) {
    const auto &cam = doc["scene"]["camera"];
    return RayTracer(num_threads,
        glm::vec3(cam["eye"][0].GetFloat(), cam["eye"][1].GetFloat(), cam["eye"][2].GetFloat()),
        glm::vec3(cam["look_at"][0].GetFloat(), cam["look_at"][1].GetFloat(), cam["look_at"][2].GetFloat()),
        glm::vec3(cam["up"][0].GetFloat(), cam["up"][1].GetFloat(), cam["up"][2].GetFloat()),
        cam["fovy"].GetInt());
}

// Parse texture image or const
TexturePtr parseTexture(const rapidjson::Value &val) {
    if (val.IsString()) {
        auto img = make_shared<Image>();
        img->loadPng(val.GetString());
        return make_shared<TextureImage>(img);
    } else if (val.IsArray()) {
        return make_shared<TextureConst>(
            glm::vec3(val[0].GetFloat(), val[1].GetFloat(), val[2].GetFloat())
        );
    }

    return nullptr;
}

// Read in generated materials from config
map<string, MaterialPtr> parseMaterials(const rapidjson::Document &doc) {
    map<string, MaterialPtr> mp;
    // TODO: Support configurable Km
    auto km = make_shared<TextureConst>(glm::vec3(0.3, 0.3, 0.3));

    if (doc.HasMember("materials") && doc["materials"].IsArray()) {
        for (const auto &mtl : doc["materials"].GetArray()) {
            // TODO: Support texture from images
            auto kd = parseTexture(mtl["kd"]);
            auto ks = parseTexture(mtl["ks"]);
            mp[mtl["name"].GetString()] = make_shared<Material>(
                kd, ks, km, mtl["mirror"].GetBool(), mtl["p"].GetFloat()
            );
        }
    }
    return mp;
}

shared_ptr<Transform> parseTransform(const rapidjson::Value &xform) {
    if (xform["type"] == "rot") {
        return TransformMatrix::rot(
            glm::vec3(
                xform["axis"][0].GetFloat(),
                xform["axis"][1].GetFloat(),
                xform["axis"][2].GetFloat()
            ),
            xform["angle"].GetFloat()
       );
    } else if (xform["type"] == "scale") {
        return TransformMatrix::scale(
            glm::vec3(
                xform["scale"][0].GetFloat(),
                xform["scale"][1].GetFloat(),
                xform["scale"][2].GetFloat()
            )
        );
    } else if (xform["type"] == "translate") {
        return TransformTrans::translate(
            glm::vec3(
                xform["s"][0].GetFloat(),
                xform["s"][1].GetFloat(),
                xform["s"][2].GetFloat()
            )
        );
    }
    return nullptr;
}

// Read in objects from config
vector<shared_ptr<Object>> parseScene(const rapidjson::Document &doc, map<string, MaterialPtr> &mtl) {
    vector<shared_ptr<Object>> v;

    // First parse and load models
    // NB: No instancing support and no caching obj files in memory, ie if two teapots are loaded
    // both need to be read from file... for now
    map<string ,string> name_to_file;
    map<string, string> name_to_dir;
    if (doc.HasMember("models") && doc["models"].IsArray()) {
        for (const auto &model : doc["models"].GetArray()) {
            name_to_file[model["name"].GetString()] = model["file"].GetString();
            name_to_dir[model["name"].GetString()] = model["dir"].GetString();
        }
    }

    // Parse scene models
    auto defMaterial = make_shared<Material>(
        make_shared<TextureConst>(glm::vec3(0.7, 0.7, 0.7)),
        make_shared<TextureConst>(glm::vec3(0.3, 0.3, 0.3)),
        make_shared<TextureConst>(glm::vec3(0.3, 0.3, 0.3)),
        true,
        100);
    for (const auto &m : doc["scene"]["models"].GetArray()) {
        string type = m["type"].GetString();
        if (type == "sphere") {
            v.emplace_back(make_shared<Sphere>(
                glm::vec3(
                    m["pos"][0].GetFloat(),
                    m["pos"][1].GetFloat(),
                    m["pos"][2].GetFloat()),
                m["scale"].GetFloat(),
                mtl[m["material"].GetString()]
            ));
        } else if (type == "plane") {
            v.emplace_back(Plane::create(
                glm::vec3(m["a"][0].GetFloat(), m["a"][1].GetFloat(), m["a"][2].GetFloat()),
                glm::vec3(m["b"][0].GetFloat(), m["b"][1].GetFloat(), m["b"][2].GetFloat()),
                glm::vec3(m["c"][0].GetFloat(), m["c"][1].GetFloat(), m["c"][2].GetFloat()),
                mtl[m["material"].GetString()]
            ));
        } else if (type == "ref") {
            // Parse transforms
            vector<shared_ptr<Transform>> xforms;
            for (const auto &xform : m["transforms"].GetArray()) {
                auto t = parseTransform(xform);
                if (t) {
                    xforms.emplace_back(t);
                }
            }

            // Merge transforms into a transform chain and load object
            auto chain = TransformChain(xforms);
            auto modelDefaultMat = m.HasMember("material") ? mtl[m["material"].GetString()] : defMaterial;
            auto objs = ObjObject::loadFromFile(
                name_to_file[m["name"].GetString()],
                name_to_dir[m["name"].GetString()],
                chain,
                modelDefaultMat
            );
            std::copy(
                std::make_move_iterator(objs.begin()),
                std::make_move_iterator(objs.end()),
                std::back_inserter(v)
            );
        } else {
            cerr << "Unknown Type: " << type << endl;
        }
    }
    return v;
}

vector<shared_ptr<Light>> parseLights(const rapidjson::Document &doc) {
    vector<shared_ptr<Light>> lights;
    if (doc["scene"].HasMember("lights") && doc["scene"]["lights"].IsArray()) {
        for (const auto &l : doc["scene"]["lights"].GetArray()) {
            lights.emplace_back(make_shared<Light>(
                glm::vec3(
                    l["pos"][0].GetFloat(),
                    l["pos"][1].GetFloat(),
                    l["pos"][2].GetFloat()),
                glm::vec3(
                    l["intensity"][0].GetFloat(),
                    l["intensity"][1].GetFloat(),
                    l["intensity"][2].GetFloat()),
                l["rad"].GetFloat()
            ));
        }
    }
    return lights;
}
