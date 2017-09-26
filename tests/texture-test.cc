#include <SimpleGL/SimpleGL.h>
#include "sgl-test.h"
#include <iostream>

int main () {
    sgl::Context ctx{700, 700, "texture test"};

    sgl::Texture2D image = sgl::TextureBuilder2D()
        .build(TEST_RES("blue.jpg"));

    sgl::MeshResource plane = sgl::createPlane();
    sgl::Shader shader = sgl::loadShader(TEST_RES("ident_vs.glsl"), TEST_RES("texture_fs.glsl"));

    glViewport(0, 0, ctx.attrs.width, ctx.attrs.height);

    while (ctx.isAlive()){
        ctx.pollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();
        shader.setTexture("image", image, 0);

        plane.bind();
        glDrawElements(GL_TRIANGLES, plane.size, GL_UNSIGNED_INT, 0);

        ctx.swapBuffers();
    }
}
