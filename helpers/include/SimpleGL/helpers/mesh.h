#pragma once
#include <SimpleGL/sglconfig.h>
#include <SimpleGL/traits.h>
#include <SimpleGL/resource.h>

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <sstream>

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
    void toObj (std::stringstream& sstream) const;
};


class MeshResource : public GLResource<GL_VERTEX_ARRAY>{
private:
    ArrayBuffer<MeshVertex> VBO;
    ElementArrayBuffer<> EBO;

public:
    size_t size;
    size_t attribs;

    MeshResource (MeshData& data);

    void release () {
        GLResource<GL_VERTEX_ARRAY>::release();
        VBO.release();
        EBO.release();
    }

};

class MeshBuilder {
public:
    MeshData data;

    void addVertex (const MeshVertex& vertex);
    void addFace (int a, int b, int c);
    int vertexCount () const;

    MeshResource build ();
};


//using MMeshResource = GLResourceM<MeshResource>;

MeshResource createPlane (int divs = 1);

} // end namespace
