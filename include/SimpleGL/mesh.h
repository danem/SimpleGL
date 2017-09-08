#pragma once
#ifdef SGL_USE_GLM

#include "sglconfig.h"
#include "traits.h"
#include "resource.h"

#include <glm/glm.hpp>
#include <vector>

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

class MeshResource {
private:
    GLResource<GL_VERTEX_ARRAY> VAO;
    GLResource<GL_ARRAY_BUFFER> VBO;
    GLResource<GL_ELEMENT_ARRAY_BUFFER> EBO;

public:
    MeshResource (const MeshData& data);

    ~MeshResource () {
        VAO.release();
        VBO.release();
        EBO.release();
    }
};

MeshResource createPlane (int divs);

} // end namespace
#endif
