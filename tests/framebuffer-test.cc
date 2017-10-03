#include <SimpleGL/helpers/SimpleGLHelpers.h>

#include "sgl-test.h"
#include <array>

int main () {
    sgl::Context ctx{500,500,"Framebuffer Test"};

    sgl::Framebuffer fbo;
    sgl::Texture2D fboTex = sgl::TextureBuilder2D()
        .build(ctx.attrs.width, ctx.attrs.height);

    fbo.attachTexture(fboTex);

    std::array<uint8_t, 500*500*3> redData{};
    for (int i = 0; i < 500*500; i++) redData[i*3] = 255;

    sgl::Texture2D redTex = sgl::TextureBuilder2D()
        .build(&redData[0], 500, 500);
    
    sgl::Shader shader = sgl::loadShader(TEST_RES("ident_vs.glsl"), TEST_RES("texture_fs.glsl"));

    sgl::MeshResource plane = sgl::createPlane();

    glViewport(0,0,ctx.attrs.width,ctx.attrs.height);
    while (ctx.isAlive()){
        ctx.pollEvents();

        {
            // Render to redTex to fboTex
            auto bg = sgl::bind_guard(fbo);
            glClear(GL_COLOR_BUFFER_BIT);

            shader.bind();
            shader.setTexture("image", redTex, 0);

            plane.bind();
            glDrawElements(GL_TRIANGLES, plane.size, GL_UNSIGNED_INT, 0);
        }

        // Display fboTex
        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();
        shader.setTexture("image", fboTex, 0);

        plane.bind();
        glDrawElements(GL_TRIANGLES, plane.size, GL_UNSIGNED_INT, 0);

        ctx.swapBuffers();
    }

}
