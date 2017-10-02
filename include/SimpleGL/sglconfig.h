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
* SGL_OPENGL_MAX_MAJOR - Define max available opengl api. Enables usage of newer features. Defaults to 3
* SGL_OPENGL_MAX_MINOR - Define max available opengl api. Enables usage of newer features. Defaults to 0
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

#if GL_VERSION_4_5
#    define SGL_OPENGL_MAX_MAJOR_DETECTED 4
#    define SGL_OPENGL_MAX_MINOR_DETECTED 5
#elif GL_VERSION_4_4
#    define SGL_OPENGL_MAX_MAJOR_DETECTED 4
#    define SGL_OPENGL_MAX_MINOR_DETECTED 4
#elif GL_VERSION_4_3
#    define SGL_OPENGL_MAX_MAJOR_DETECTED 4
#    define SGL_OPENGL_MAX_MINOR_DETECTED 3
#elif GL_VERSION_4_2
#    define SGL_OPENGL_MAX_MAJOR_DETECTED 4
#    define SGL_OPENGL_MAX_MINOR_DETECTED 2
#elif GL_VERSION_4_1
#    define SGL_OPENGL_MAX_MAJOR_DETECTED 4
#    define SGL_OPENGL_MAX_MINOR_DETECTED 1
#elif GL_VERSION_4_0
#    define SGL_OPENGL_MAX_MAJOR_DETECTED 4
#    define SGL_OPENGL_MAX_MINOR_DETECTED 0
#elif GL_VERSION_3_3
#    define SGL_OPENGL_MAX_MAJOR_DETECTED 3
#    define SGL_OPENGL_MAX_MINOR_DETECTED 3
#elif GL_VERSION_3_2
#    define SGL_OPENGL_MAX_MAJOR_DETECTED 3
#    define SGL_OPENGL_MAX_MINOR_DETECTED 2
#elif GL_VERSION_3_1
#    define SGL_OPENGL_MAX_MAJOR_DETECTED 3
#    define SGL_OPENGL_MAX_MINOR_DETECTED 1
#elif GL_VERSION_3_0
#    define SGL_OPENGL_MAX_MAJOR_DETECTED 3
#    define SGL_OPENGL_MAX_MINOR_DETECTED 0
#elif GL_VERSION_2_1
#    define SGL_OPENGL_MAX_MAJOR_DETECTED 2
#    define SGL_OPENGL_MAX_MINOR_DETECTED 1
#elif GL_VERSION_2_0
#    define SGL_OPENGL_MAX_MAJOR_DETECTED 2
#    define SGL_OPENGL_MAX_MINOR_DETECTED 0
#endif


#ifndef SGL_OPENGL_MAX_MAJOR
#   define SGL_OPENGL_MAX_MAJOR SGL_OPENGL_MAX_MAJOR_DETECTED
#endif

#ifndef SGL_OPENGL_MAX_MINOR
#   define SGL_OPENGL_MAX_MINOR SGL_OPENGL_MAX_MINOR_DETECTED
#endif

#define SGL_OPENGL_VER(major,minor) ((SGL_OPENGL_MAX_MAJOR >= major) && (SGL_OPENGL_MAX_MINOR >= minor))

#if SGL_OPENGL_VER(4,4)
#   define SGL_BUFFERSTORAGE_SUPPORTED 1
#endif

#if SGL_OPENGL_VER(4,3)
#   define SGL_COMPUTESHADER_SUPPORTED 1
#endif

#if SGL_OPENGL_VER(4,1)
#   define SGL_PROGRAMPIPELINES_SUPPORTED 1
#endif

#if SGL_OPENGL_VER(3,3)
#   define SGL_UNIFORMBLOCK_SUPPORTED 1
#endif

#if SGL_OPENGL_VER(3,0)
#   define SGL_RENDERBUFFER_SUPPORTED 1
#   define SGL_FRAMEBUFFER_SUPPORTED 1
#   define SGL_VERTEXARRAY_SUPPORTED 1
#endif

namespace sgl {
namespace detail {

} // namespace
} // namespace

#endif // SGLCONFIG_H
