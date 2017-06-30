#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <vector>
#include <memory>

using std::vector;


// TODO: pos/norm should be pure virtual, but initializer_list cannot construct instances of Transform
class Transform {
public:
    Transform() { }
    virtual ~Transform() { }
    virtual glm::vec3 pos(const glm::vec3 &pos) const { return pos; }
    virtual glm::vec3 norm(const glm::vec3 &norm) const { return norm; }
};


typedef std::shared_ptr<Transform> TransformPtr;


class TransformChain : public Transform {
public:
    TransformChain(std::initializer_list<TransformPtr> xforms) : Transform(), xforms(xforms) { }
    TransformChain(const std::vector<TransformPtr> &xforms) : Transform(), xforms(xforms) { }

    virtual ~TransformChain() { }
    glm::vec3 pos(const glm::vec3 &pos) const;
    glm::vec3 norm(const glm::vec3 &norm) const;
private:
    vector<TransformPtr> xforms;
};


class TransformMatrix : public Transform {
public:
    TransformMatrix(const glm::mat3 &M) :
        Transform(), M(M), N(glm::transpose(glm::inverse(M))) { }
    glm::vec3 pos(const glm::vec3 &pos) const;
    glm::vec3 norm(const glm::vec3 &norm) const;
public:
    static TransformPtr rot(const glm::vec3 &rot, float deg);
    static TransformPtr scale(const glm::vec3 &scale);
private:
    glm::mat3 M;
    glm::mat3 N;
};


class TransformTrans : public Transform {
public:
    TransformTrans(const glm::vec3 &trans) : Transform(), s(trans) { }
    glm::vec3 pos(const glm::vec3 &pos) const { return pos + s; }
    glm::vec3 norm(const glm::vec3 &norm) const { return norm; }
public:
    static TransformPtr translate(const glm::vec3 &trans);
private:
    glm::vec3 s;
};
#endif
