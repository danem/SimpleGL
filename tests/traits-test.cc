#include <SimpleGL/SimpleGL.h>
#include <iostream>

int main () {
    std::cout << "GL_TEXTURE_1D != buffer: " << !sgl::traits::IsBuffer<GL_TEXTURE_1D>::value << std::endl;
    std::cout << "GL_ARRAY_BUFFER == writable: " << sgl::traits::IsWritable<GL_ARRAY_BUFFER>::value << std::endl;
    std::cout << "GL_TEXTURE_2D == writable: " << sgl::traits::IsWritable<GL_TEXTURE_2D>::value << std::endl;
    std::cout << "GL_TEXTURE_2D != tex1d: " << !sgl::traits::IsTex1D<GL_TEXTURE_2D>::value << std::endl;
    std::cout << "gl_type<float[]> == GL_FLOAT: " << (sgl::traits::gl_type<float[]>() == GL_FLOAT) << std::endl;
    std::cout << "gl_type<float> == GL_FLOAT: " << (sgl::traits::gl_type<float>() == GL_FLOAT) << std::endl;
}
