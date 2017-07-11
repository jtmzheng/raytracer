#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "objobject.hpp"
#include "material.hpp"
#include "transform.hpp"
#include "texture.hpp"

#include <glm/ext.hpp>
#include <iostream>
#include <map>

using namespace std;


map<int, glm::vec3> getOrCreateNormals(
    const vector<tinyobj::shape_t> &shapes,
    const tinyobj::attrib_t &attrib);


// NB: Maps vertex index (from obj file) to normals (either created or loaded)
map<int, glm::vec3> getOrCreateNormals(
    const vector<tinyobj::shape_t> &shapes,
    const tinyobj::attrib_t &attrib)
{
    map<int, glm::vec3> mpVertNorm; // Map vertex id to normal (1 to 1)
    map<int, int> mpVertCount;      // Number of faces vertex belongs in
    for (size_t s = 0; s < shapes.size(); ++s) {
        for (size_t i = 0; i < shapes[s].mesh.indices.size()/3; ++i) {
            if (shapes[s].mesh.indices[3*i].normal_index == -1) {
                // Generate (not encoded in obj file)
                vector<glm::vec3> v(3);
                for (uint k = 0; k < v.size(); ++k) {
                    auto idx = shapes[s].mesh.indices[3*i + k];
                    auto vidx = idx.vertex_index;
                    v[k] = glm::vec3(
                        attrib.vertices[3*vidx],
                        attrib.vertices[3*vidx+1],
                        attrib.vertices[3*vidx+2]
                    );
                }
                // NB: Assumes winding order is correct
                auto faceNorm = glm::normalize(glm::cross(v[1]-v[0], v[2]-v[0]));
                for (uint k = 0; k < v.size(); ++k) {
                    auto idx = shapes[s].mesh.indices[3*i + k];
                    auto vidx = idx.vertex_index;
                    mpVertNorm[vidx] += faceNorm;
                    mpVertCount[vidx]++;
                }
            } else {
                // Load and normalize (not guarenteed to be normalized in standard)
                for (uint k = 0; k < 3; ++k) {
                    auto idx = shapes[s].mesh.indices[3*i + k];
                    auto vidx = idx.vertex_index;
                    auto nidx = idx.normal_index;
                    mpVertNorm[vidx] = glm::normalize((glm::vec3(
                        attrib.normals[3*nidx],
                        attrib.normals[3*nidx+1],
                        attrib.normals[3*nidx+2]
                    )));
                }
            }
        }
    }

    // Divide to get the average for each generated normal
    for (const auto &pr : mpVertCount) {
        mpVertNorm[pr.first] /= pr.second;
    }
    cout << "Normals: " << mpVertNorm.size() << endl;
    return mpVertNorm;
}

vector<shared_ptr<ObjObject>> ObjObject::loadFromFile(
    string file,
    string base,
    const Transform &xform,
    const MaterialPtr &def)
{
    vector<shared_ptr<ObjObject>> objs;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;
    tinyobj::attrib_t attrib;
    string err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file.c_str(), base.c_str())) {
        cerr << "Error loadFromFile: " << err << endl;
        return objs;
    }

    // TODO: Kludge constant km texture
    auto km = make_shared<TextureConst>(glm::vec3(0.3, 0.3, 0.3));

    // Map the tinyob::material into Material instances
    vector<MaterialPtr> mats(materials.size(), nullptr);
    for (size_t i = 0; i < materials.size(); ++i) {
        glm::vec3 kd(materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
        glm::vec3 ks(materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
        // TODO: Probably pass "mirror" and "p" config down from config (constant for now)
        mats[i] = make_shared<Material>(
            make_shared<TextureConst>(kd), make_shared<TextureConst>(ks), km, true, 100
        );
    }

    // Add default material
    mats.emplace_back(def);

    // TODO: No support for texture coordinate right now
    auto normals = getOrCreateNormals(shapes, attrib); // Model space
    for (size_t s = 0; s < shapes.size(); ++s) {
        auto obj = make_shared<ObjObject>();
        for (size_t i = 0; i < shapes[s].mesh.indices.size()/3; ++i) {
            int mid = shapes[s].mesh.material_ids[i];
            if (mid < 0 || mid >= static_cast<int>(mats.size())) {
                // Use default material.
                mid = mats.size() - 1;
            }

            // Always normalize normals here since obj standard doesnt normalize
            vector<Vertex> verts;
            for (uint k = 0; k < 3; ++k) {
                auto idx = shapes[s].mesh.indices[3*i + k];
                auto vidx = idx.vertex_index;
                auto tidx = idx.texcoord_index;
                auto pos = xform.pos(glm::vec3(
                    attrib.vertices[3*vidx],
                    attrib.vertices[3*vidx+1],
                    attrib.vertices[3*vidx+2]
                ));
                verts.emplace_back(
                    pos,
                    glm::normalize(xform.norm(normals[vidx])),
                    tidx == -1 ? glm::vec2() : glm::vec2(
                        attrib.texcoords[2*tidx],
                        attrib.texcoords[2*tidx+1]
                    )
                );

                // Update the AABB
                obj->box.update(pos);
            }
            obj->triangles.emplace_back(make_shared<Triangle>(verts[0], verts[1], verts[2], mats[mid]));
        }

        // TODO: This is a ghetto way of adding the bvh to the obj
        obj->bvh = make_shared<Bvh>(obj->triangles);
        objs.emplace_back(obj);
    }

    return objs;
}


bool ObjObject::intersect(
    const glm::vec3 &eye,
    const glm::vec3 &dir,
    HitRecord &minHr,
    const std::pair<float, float> &rng) const
{
    // Use linear search if no bvh for obj
    if (bvh != nullptr) {
        return bvh->intersect(eye, dir, minHr, rng);
    } else {
        auto tx = calcT(eye.x, dir.x, box.x);
        auto ty = calcT(eye.y, dir.y, box.y);
        auto tz = calcT(eye.z, dir.z, box.z);
        if (tx.first > ty.second || tx.first > tz.second) return false;
        if (ty.first > tx.second || ty.first > tz.second) return false;
        if (tz.first > ty.second || tz.first > tx.second) return false;

        for (const auto &triangle : triangles) {
            triangle->intersect(eye, dir, minHr, rng);
        }
    }

    return minHr.surf != nullptr;
}
