#ifndef SGLCONFIG_H
#define SGLCONFIG_H

/**
*
* Compile Time configuration flags:
* SGL_USE_GLEW         - Use GLEW for opengl support
* SGL_USE_GLFW         - Use GLFW for context creation and windowing
* SGL_USE_GLM          - Use GLM math library. Needed for Camera
* SGL_DEBUG [1-3]      - Enable debugging messages and error checking
* SGL_DEBUG_STATS      - Collect gl runtime information
* SGL_OPENGL_MAX_MAJOR - Define default max available opengl api. Enables usage of newer features.
* SGL_OPENGL_MAX_MINOR - Define default max available opengl api. Enables usage of newer features.
*/

#if defined(SGL_USE_GLEW)
#   define GLEW_STATIC
#   include <GL/glew.h>
#else
#   error "Only GLEW is currently supported. Make sure to define -DSGL_USE_GLEW"
#endif

#if defined(SGL_USE_GLFW)
#   include <GLFW/glfw3.h>
#else
#   error "Only GLFW3 is currently supported. Make sure to define -DSGL_USE_GLFW"
#endif

#if defined(__APPLE__) || defined(__MACH__)
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#elif defined(__LINUX__) || defined(__linux__)
#   include <GL/gl.h>
#   include <GL/glu.h>
#else
#   include <GL/gl.h>
#   include <GL/glu.h>
#endif

#ifndef SGL_OPENGL_MAX_MAJOR
#   define SGL_OPENGL_MAX_MAJOR 4
#endif

#ifndef SGL_OPENGL_MAX_MINOR
#   define SGL_OPENGL_MAX_MINOR 1
#endif

namespace sgl {
namespace config {
    struct SGL_OPENGL_STATE {
        int version_major = SGL_OPENGL_MAX_MAJOR;
        int version_minor = SGL_OPENGL_MAX_MINOR;
    };
    static SGL_OPENGL_STATE __sglOpenGLState__;

    inline bool sglOpenglVersion (int major, int minor) {
        return __sglOpenGLState__.version_major >= major && __sglOpenGLState__.version_minor == minor;
    }
} // end namespace

inline void sglInitialize (int major, int minor) {
    config::__sglOpenGLState__.version_major = major;
    config::__sglOpenGLState__.version_minor = minor;
}
} // namespace

#define SGL_RENDERBUFFER_SUPPORTED     sgl::config::sglOpenglVersion(3,0)
#define SGL_FRAMEBUFFER_SUPPORTED      sgl::config::sglOpenglVersion(3,0)
#define SGL_VERTEXARRAY_SUPPORTED      sgl::config::sglOpenglVersion(3,0)
#define SGL_UNIFORMBLOCK_SUPPORTED     sgl::config::sglOpenglVersion(3,3)
#define SGL_PROGRAMPIPELINES_SUPPORTED sgl::config::sglOpenglVersion(4,1)
#define SGL_COMPUTESHADER_SUPPORTED    sgl::config::sglOpenglVersion(4,3)
#define SGL_BUFFERSTORAGE_SUPPORTED    sgl::config::sglOpenglVersion(4,4)
#define SGL_RW                         (SGL_BUFFERSTORAGE_SUPPORTED ? (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT) : GL_DYNAMIC_DRAW)



#endif // SGLCONFIG_H
