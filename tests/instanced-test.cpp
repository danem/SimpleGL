#include <SimpleGL/SimpleGL.h>
#include "sgl-test.h"
#include <vector>
#include <glm/glm.hpp>

// TODO: The UV coordinates seem to be wrong...
int main () {
    sgl::Context ctx{500,500,"instancing test"};
    sgl::Shader shader = sgl::loadShader(TEST_RES("instanced_vs.glsl"), TEST_RES("fs.glsl"));

    std::vector<glm::vec2> offsets;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++){
            offsets.emplace_back(i/10.f, j/10.f);
        }
    }
    sgl::ArrayBuffer<glm::vec2> offsetsBuf{offsets};
    sgl::MeshResource mesh = sgl::createPlane();

    sgl::VertexAttribBuilder(mesh,3,1)
        .add(offsetsBuf);

    glViewport(0,0,ctx.attrs.width,ctx.attrs.height);
    while (ctx.isAlive()){
        glClear(GL_COLOR_BUFFER_BIT);
        ctx.pollEvents();

        shader.bind();
        mesh.bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.size, offsets.size());

        ctx.swapBuffers();
    }
}
