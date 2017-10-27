#include "../include/SimpleGL/sglconfig.h"

using namespace sgl;
using namespace sgl::config;

struct SGL_OPENGL_STATE {
    int version_major = SGL_OPENGL_MAX_MAJOR;
    int version_minor = SGL_OPENGL_MAX_MINOR;
};

static SGL_OPENGL_STATE __sglOpenGLState__;

bool sgl::config::sglOpenglVersion (int major, int minor) {
    return __sglOpenGLState__.version_major >= major && __sglOpenGLState__.version_minor >= minor;
}

void sgl::sglInitialize (int major, int minor) {
    __sglOpenGLState__.version_major = major;
    __sglOpenGLState__.version_minor = minor;
#if SGL_DEBUG >= 1
    printf("SGL OpenGL Version %d.%d\n", major, minor);
#endif
}
