#ifdef SGL_USE_GLM
#include <SimpleGL/mesh.h>

using namespace sgl;

void MeshBuilder::addVertex (const MeshVertex& vertex) {
    data.vertices.emplace_back(vertex);
}

void MeshBuilder::addFace (int a, int b, int c) {
    data.indices.emplace_back(a);
    data.indices.emplace_back(b);
    data.indices.emplace_back(c);
}

int MeshBuilder::vertexCount () const {
    return data.indices.size();
}

MeshResource::MeshResource (MeshData& data) :
    size(data.indices.size())
{
    auto bg = sgl::bind_guard(*this);

    sgl::bufferData(EBO, data.indices);
    sgl::bufferData(VBO, data.vertices);
    
    sgl::VertexAttribBuilder(*this)
        .add<glm::vec3, glm::vec2, glm::vec3>(VBO);
}

sgl::MeshResource sgl::createPlane (int divs) {
    float scale = 1.0f / divs;
    float hd = divs / 2.0f;
    MeshBuilder m;
    for (int i = 0; i < divs+1; i++){
        for (int j = 0; j < divs+1; j++){
            glm::vec3 pos((i-hd)*scale*2, (j-hd)*scale*2,0);
            glm::vec2 uv((float)i / (float)divs, (float)j / (float)divs);
            MeshVertex mv;
            mv.position = pos;
            mv.uvcoord = uv;
            mv.normal = glm::vec3(0,1,0);
            m.addVertex(mv);
        }
    }

    for (int i = 0; i < divs; i++){
        for (int j = 0; j < divs; j++){
            int tl = i * (divs+1) + j;
            int bl = tl + 1;
            int tr = (i+1) * (divs+1) + j;
            int br = tr + 1;
            m.addFace(bl,br,tr);
            m.addFace(bl,tr,tl);
        }
    }
    return MeshResource(m.data);
}





#endif

