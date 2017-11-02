#include "../include/SimpleGL/helpers/mesh.h"

using namespace sgl;


void MeshData::toObj (std::stringstream& sstream) const {
    std::stringstream verts;
    std::stringstream uvs;
    std::stringstream norms;

    for (const auto& vert : vertices) {
        verts << "v "  << vert.position.x << " " << vert.position.y << " " << vert.position.z << "\n";
        uvs   << "vt " << vert.uvcoord.x  << " " << vert.uvcoord.y << "\n";
        norms << "vn " << vert.normal.x   << " " << vert.normal.y << " " << vert.normal.z << "\n";
    }

    sstream << verts.str() << "\n"
            << uvs.str() << "\n"
            << norms.str() << "\n";

    for (size_t i = 0; i < indices.size(); i+=3) {
        sstream << "f "
                << indices[i] << "/" << indices[i] << "/" << indices[i] << " "
                << indices[i+1] << "/" << indices[i+1] << "/" << indices[i+1] << " "
                << indices[i+2] << "/" << indices[i+2] << "/" << indices[i+2] << "\n";
                   /*
                << indices[i] + 1 << " "
                << indices[i+1] + 1 << " "
                << indices[i+2] + 1 << "\n";
                           */
    }
}

void MeshBuilder::addVertex (const MeshVertex& vertex) {
    data.vertices.push_back(vertex);
}

void MeshBuilder::addFace (int a, int b, int c) {
    data.indices.push_back(a);
    data.indices.push_back(b);
    data.indices.push_back(c);
}

int MeshBuilder::vertexCount () const {
    return data.indices.size();
}

MeshResource MeshBuilder::build () {
    return {data};
}

MeshResource::MeshResource (MeshData& data) :
    size(data.indices.size()),
    attribs(3)
{
    sgl::bufferData(VBO, data.vertices, GL_STATIC_DRAW);
    sgl::bufferData(EBO, data.indices, GL_STATIC_DRAW);

    sgl::VertexAttribBuilder(*this)
        .addElementBuffer(EBO)
        .addBuffer<sgl::vec3f, sgl::vec2f, sgl::vec3f>(VBO)
        .commit();
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





