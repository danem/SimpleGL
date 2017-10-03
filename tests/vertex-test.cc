#include <SimpleGL/helpers/SimpleGLHelpers.h>
#include "sgl-test.h"
#include <glm/glm.hpp>

int main () {
    sgl::Context ctx {500,500,"vertex text"};

    sgl::Shader shader = sgl::loadShader(TEST_RES("ident_vs.glsl"), TEST_RES("fs.glsl"));

    float verts[] = {
        -0.5f, -0.5f, 0,
        0.5f, -0.5f, 0,
        -0.5f, 0.5f, 0,
        0.5f, 0.5f, 0
    };

    float uvs[] = {
        0, 0,
        1, 0,
        0, 1,
        1, 1
    };

    float interleaved [] = {
        -0.5f, -0.5f, 0, 0, 0,
        0.5f, -0.5f,  0, 1, 0,
        -0.5f, 0.5f,  0, 0, 1,
        0.5f, 0.5f,   0, 1, 1
    };

    uint32_t idxs[] = {
        0, 2, 1,
        0, 3, 2
    };

    sgl::ArrayBuffer<float> ibuf{interleaved, 20};
    sgl::ArrayBuffer<float> vbuf{verts, 12};
    sgl::ArrayBuffer<float> ubuf{uvs, 8};
    sgl::ElementArrayBuffer<> ebuf{idxs, 6};

    sgl::VertexArray vao;

    sgl::vertexAttribBuilder(vao)
            .addElementBuffer(ebuf)
            .addBuffer<sgl::vec3f>(vbuf)
            .addBuffer<sgl::vec2f>(ubuf)
            .commit();

    sglCatchGLError();


    glViewport(0,0,ctx.attrs.width,ctx.attrs.height);
    while (ctx.isAlive()) {
        ctx.pollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();
        vao.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        ctx.swapBuffers();
    }


}
