#include <SimpleGL/helpers/SimpleGLHelpers.h>
#include "sgl-test.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "contrib/stb_image.h"

int main () {
    sgl::Context ctx{700, 700, "texture test"};

    int x, y, n;
    unsigned char * img = stbi_load(TEST_RES("blue.jpg"),&x,&y,&n,0);

    sgl::Texture2D image = sgl::TextureBuilder2D()
        .build(img, x, y);

    stbi_image_free(img);

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
