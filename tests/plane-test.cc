#include <SimpleGL/SimpleGL.h>
#include "sgl-test.h"

int main () {
    sgl::Context ctx{500, 500, "static plane test"};

    sgl::Shader shader = sgl::loadShader(TEST_RES("vs.glsl"), TEST_RES("fs.glsl"));

    sgl::MeshResource plane = sgl::createPlane(1);

    sgl::PerspectiveCamera cam(ctx.attrs.width, ctx.attrs.height);
    sgl::CameraController cc(ctx.attrs.width, ctx.attrs.height, cam);
    ctx.addMouseHandler(cc);
    cam.position = {0,0,-3};
    cam.update();

    glViewport(0, 0, ctx.attrs.width, ctx.attrs.height);
    while (ctx.isAlive()){
        glClear(GL_COLOR_BUFFER_BIT);
        ctx.pollEvents();

        auto sg = sgl::bind_guard(shader);
        shader.setUniformMatrix4f("cameraMat", cam.getProjection());
        shader.setUniformMatrix4f("viewMat", cam.getView());

        auto pg = sgl::bind_guard(plane);
        glDrawElements(GL_TRIANGLES, plane.size, GL_UNSIGNED_INT, 0);

        //sglCatchGLError();
        sglCheckGLError();
        ctx.swapBuffers();
    }
}


