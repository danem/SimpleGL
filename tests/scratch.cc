#include <SimpleGL/SimpleGL.h>

int main () { 
    sgl::Context ctx{500, 500};

    
    glViewport(0,0,ctx.attrs.width, ctx.attrs.height);
    while (ctx.isAlive()){
        ctx.pollEvents();
        ctx.swapBuffers();
    }
}
