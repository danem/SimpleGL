#include <SimpleGL/SimpleGL.h>
#include "sgl-test.h"
#include <array>
#include <iostream>


int main () {
    sgl::Context ctx{500,500,"pbo test"};
    sgl::MeshResource renderQuad = sgl::createPlane(1);

    std::array<char,500*500*3> data;
    sgl::Shader shader = sgl::loadShader(TEST_RES("ident_vs.glsl"), TEST_RES("texture_fs.glsl"));

    sgl::Texture2D tex = sgl::TextureBuilder2D()
        .build(ctx.attrs.width, ctx.attrs.height);

    sgl::PBOUploader uploader(tex);

    int frame = 0;
    glViewport(0,0,500,500);

    while (ctx.isAlive()){
        ctx.pollEvents();

        frame = (frame + 1) % 255;
        for (size_t i = 0; i < data.size(); i++){
            data[i] = frame;
        }
        uploader.update(data);

        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();
        shader.setTexture("image", tex, 0);

        renderQuad.bind();
        glDrawElements(GL_TRIANGLES, renderQuad.size, GL_UNSIGNED_INT, 0);

        ctx.swapBuffers();
    }
}
