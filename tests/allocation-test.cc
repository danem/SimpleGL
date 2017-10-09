#define SGL_DEBUG 3
#include <SimpleGL/helpers/SimpleGLHelpers.h>
#include <vector>
#include <array>

int main () {
    sgl::Context ctx{500,500,"allocation"};
    std::vector<sgl::GLResource<GL_ARRAY_BUFFER>> buf;
    std::vector<sgl::GLResource<GL_ARRAY_BUFFER>> buf2(2);
    std::array<sgl::GLResource<GL_ARRAY_BUFFER>,10> buf3;

}
