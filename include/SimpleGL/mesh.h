#pragma once
#ifdef SGL_USE_GLM

#include "sglconfig.h"
#include "traits.h"
#include "resource.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace sgl {

struct MeshVertex {
    glm::vec3 position;
    glm::vec2 uvcoord;
    glm::vec3 normal;
};

struct MeshData {
public:
    std::vector<GLuint> indices;
    std::vector<MeshVertex> vertices;
};

class MeshBuilder {
public:
    MeshData data;

    void addVertex (const MeshVertex& vertex);
    void addFace (int a, int b, int c);
    int vertexCount () const;
};

class MeshResource : public GLResource<GL_VERTEX_ARRAY>{
private:
    ArrayBuffer<MeshVertex> VBO;
    ElementArrayBuffer<> EBO;

public:
    size_t size;

    MeshResource (MeshData& data);

    void release () {
        GLResource<GL_VERTEX_ARRAY>::release();
        VBO.release();
        EBO.release();
    }

};

using MMeshResource = GLResourceM<MeshResource>;

MeshResource createPlane (int divs = 1);

} // end namespace
#endif
