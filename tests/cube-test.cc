#include <SimpleGL/SimpleGL.h>
#include "sgl-test.h"

int main () {
    sgl::Context ctx{500,500,"cube test"};

    sgl::Shader shader = sgl::loadShader(TEST_RES("vs.glsl"), TEST_RES("fs.glsl"));
    sgl::MeshResource mesh = sgl::createCube();

    sgl::PerspectiveCamera cam{ctx.attrs.width, ctx.attrs.height};
    cam.position = {0,0,-3};
    cam.looking = {0,0,0};
    cam.update();

    glViewport(0,0,ctx.attrs.width,ctx.attrs.height);
    while (ctx.isAlive()){
        ctx.pollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();
        shader.setUniformMatrix4f("cameraMat", cam.getProjection());
        shader.setUniformMatrix4f("viewMat", cam.getView());

        mesh.bind();
        glDrawElements(GL_LINE_LOOP, mesh.size, GL_UNSIGNED_INT, 0);
        
        ctx.swapBuffers();
    }
}


