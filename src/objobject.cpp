#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "objobject.hpp"
#include "material.hpp"
#include "transform.hpp"

#include <glm/ext.hpp>
#include <iostream>

vector<shared_ptr<ObjObject>> ObjObject::loadFromFile(string file, string base, const Transform &xform) {
    vector<shared_ptr<ObjObject>> objs;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;
    tinyobj::attrib_t attrib;
    string err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file.c_str(), base.c_str())) {
        cerr << "Error loadFromFile: " << err << endl;
        return objs;
    }

    // Map the tinyob::material into Material instances
    vector<MaterialPtr> mats(materials.size(), nullptr);
    for (size_t i = 0; i < materials.size(); ++i) {
        glm::vec3 kd(materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
        glm::vec3 ks(materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
        mats[i] = make_shared<Material>(kd, ks, true, 100);
    }

    // Add default material
    mats.emplace_back(make_shared<Material>(glm::vec3(0.7, 0.7, 0.7), glm::vec3(0.3, 0.3, 0.3), true, 100));

    // TODO: No support for texture coordinate right now
    // TODO: Generate normals if none existing
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
                auto nidx = idx.normal_index;
                auto pos = xform.pos(glm::vec3(
                    attrib.vertices[3*vidx],
                    attrib.vertices[3*vidx+1],
                    attrib.vertices[3*vidx+2]
                ));
                verts.emplace_back(
                    pos,
                    glm::normalize(xform.norm(glm::vec3(
                        attrib.normals[3*nidx],
                        attrib.normals[3*nidx+1],
                        attrib.normals[3*nidx+2]
                    ))),
                    glm::vec2()
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
