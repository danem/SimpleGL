#include <SimpleGL/SimpleGL.h>
#include <iostream>
#include <mutex>

int main () {
    std::cout << "sizeof(GLuint): " << sizeof(GLuint) << std::endl;
    std::cout << "sizeof(GLResource<GL_FRAMEBUFFER>): " << sizeof(sgl::GLResource<GL_FRAMEBUFFER>) << std::endl;
    std::cout << "sizeof(GLResource<GL_ARRAY_BUFFER>): " << sizeof(sgl::GLResource<GL_ARRAY_BUFFER>) << std::endl;
    std::cout << "sizeof(GLInterface<GL_FRAMEBUFFER>): " << sizeof(sgl::detail::GLInterface<GL_FRAMEBUFFER>) << std::endl;
    std::cout << "sizeof(Texture<GL_TEXTURE_2D>): " << sizeof(sgl::Texture<GL_TEXTURE_2D>) << std::endl;
    std::cout << "sizeof(BindGuard<GL_FRAMEBUFFER>): " << sizeof(sgl::BindGuard<GL_FRAMEBUFFER>) << std::endl;
    std::cout << "sizeof(Shader): " << sizeof(sgl::Shader) << std::endl;
    std::cout << "sizeof(mutex): " << sizeof(std::lock_guard<char>) << std::endl;
}
