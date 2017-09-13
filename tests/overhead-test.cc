#include <SimpleGL/SimpleGL.h>

int main () {
    // Meant to look at under decompiler
    sgl::GLResource<GL_ELEMENT_ARRAY_BUFFER> buf;
    sgl::GLResource<GL_ARRAY_BUFFER> buf2;
    auto bg1 = sgl::bind_guard(buf);
    buf.release();
    buf2.release();
}
