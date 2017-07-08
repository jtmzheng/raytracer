#ifndef PARSER_H
#define PARSER_H

#include "raytracer.hpp"
#include "transform.hpp"
#include "rapidjson/document.h"

// Parse image dimensions
pair<int, int> parseImageDim(const rapidjson::Document &doc);

// Initialize raytracer camera using json config
RayTracer parseSceneCamera(const rapidjson::Document &doc, uint num_threads);

// Read in generated materials from config
map<string, MaterialPtr> parseMaterials(const rapidjson::Document &doc);

// Read in objects from config
vector<shared_ptr<Object>> parseScene(const rapidjson::Document &doc, map<string, MaterialPtr> &mtl);

// Read in lights from doc
vector<shared_ptr<Light>> parseLights(const rapidjson::Document &doc);

// Read in transform
shared_ptr<Transform> parseTransform( const rapidjson::Value &val);
#endif
