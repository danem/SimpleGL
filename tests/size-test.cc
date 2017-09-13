#include <SimpleGL/SimpleGL.h>
#include <iostream>
#include <mutex>

int main () {
    std::cout << "sizeof(bool): " << sizeof(bool) << std::endl;
    std::cout << "sizeof(GLuint): " << sizeof(GLuint) << std::endl;
    std::cout << "sizeof(GLenum): " << sizeof(GLenum) << std::endl;
    std::cout << "sizeof(GLResource<GL_FRAMEBUFFER>): " << sizeof(sgl::GLResource<GL_FRAMEBUFFER>) << std::endl;
    std::cout << "sizeof(GLResource<GL_ARRAY_BUFFER>): " << sizeof(sgl::GLResource<GL_ARRAY_BUFFER>) << std::endl;
    std::cout << "sizeof(Shader): " << sizeof(sgl::Shader) << std::endl;
    std::cout << "sizeof(GLInterface<GL_FRAMEBUFFER>): " << sizeof(sgl::detail::GLInterface<GL_FRAMEBUFFER>) << std::endl;
    std::cout << "sizeof(Texture<GL_TEXTURE_2D>): " << sizeof(sgl::Texture<GL_TEXTURE_2D>) << std::endl;
    std::cout << "sizeof(BindGuard<GL_FRAMEBUFFER>): " << sizeof(sgl::BindGuard<GL_FRAMEBUFFER>) << std::endl;
    std::cout << "sizeof(Param<float>): " << sizeof(sgl::Param<float>) << std::endl;
    std::cout << "traits::param_size<float,float,float>::size: " << sgl::traits::param_size<float,float,float>::size << std::endl;
}
