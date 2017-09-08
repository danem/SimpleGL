#include <SimpleGL/SimpleGL.h>
#include <iostream>

int main () {
    sgl::Context ctx{500,500,"texture test"};
    
    sgl::Texture2D texture = sgl::TextureBuilder<GL_TEXTURE_2D>()
        .build(500,500);

    sgl::Texture3D texture2 = sgl::TextureBuilder<GL_TEXTURE_3D>()
        .build(500,500,500);
    
    glViewport(0,0,ctx.attrs.width,ctx.attrs.height);

    std::cout << "gluint size: " << sizeof(GLuint) << std::endl;
    std::cout << "res size: " << sizeof(sgl::GLResource<GL_FRAMEBUFFER>) << std::endl;
    std::cout << "tex size: " << sizeof(sgl::Texture1D) << std::endl;


    while (ctx.isAlive()){
        ctx.pollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        ctx.swapBuffers();
    }
}
