#include <SimpleGL/SimpleGL.h>
#include <vector>

int main () {
    sgl::Context ctx{500, 500, "test"};
    glViewport(0,0, ctx.attrs.width, ctx.attrs.height);

    /*
    std::vector<sgl::SFramebuffer> fbos;
    for (int i = 0; i < 5; i++){
        fbos.push_back(sgl::shared_resource<GL_FRAMEBUFFER>());
    }

    while (ctx.isAlive()){
        ctx.pollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        ctx.swapBuffers();
    }
    */
}
