#include <SimpleGL/helpers/SimpleGLHelpers.h>

int main () {
    sgl::Context ctx = sgl::ContextBuilder()
        .setSize(500, 500)
        .setTitle("key test")
        .build();

    ctx.addKeyHandler([&ctx](const sgl::KeyEvent& event){
        if (event.key == 96) ctx.close();
    });

    while (ctx.isAlive()) {
        ctx.pollEvents();
        ctx.swapBuffers();
    }
}
