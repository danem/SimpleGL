#include <SimpleGL/SimpleGL.h>

int main () {
    sgl::Context ctx{500,500,"test"};

    glViewport(0, 0, ctx.attrs.width, ctx.attrs.height);

    while (ctx.isAlive()){
        ctx.pollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        ctx.swapBuffers();
    }
}
