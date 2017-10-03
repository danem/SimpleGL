#include <SimpleGL/helpers/SimpleGLHelpers.h>

int main () {
    sgl::Context ctx{500,500,"shader test"};
    
    // This shouldn't compile
    sgl::Framebuffer framebuffer;

    sgl::Shader shader{0};
    //shader.setTexture("foo", framebuffer, 0);
}
