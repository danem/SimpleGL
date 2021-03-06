#ifndef SGLCONFIG_H
#define SGLCONFIG_H

/**
* Compile Time configuration flags:
* SGL_USE_GLES         - Enable GLES
* SGL_OPENGL_MAX_MAJOR - Define default max available opengl api. Used by SimpleGLHelper Context Creation
* SGL_OPENGL_MAX_MINOR - Define default max available opengl api. Used by SimpleGLHelper Context Creation
* SGL_DEBUG [1-3]      - Enable debugging messages and error checking.
*                        If OpenGL 4.3 is available consider using sgl::utils::initializeDebugging
* SGL_NO_GL            - Disable inclusion of all opengl related headers. Useful for including sgl
*                        in another project.
*/

#ifndef SGL_NO_GL
#    ifndef SGL_USE_GLES
#        include <epoxy/gl.h>
#    endif

#    if defined(__APPLE__) || defined(__MACH__)
#       include <OpenGL/gl.h>
#       include <OpenGL/glu.h>
#    elif defined(__ANDROID__)
#       include <GLES3/gl3.h>
#       include <GLES3/gl3ext.h>
#       include <GLES3/gl31.h>
#    elif defined(__LINUX__) || defined(__linux__)
#       include <GL/gl.h>
#       include <GL/glu.h>
#    elif defined(_WIN32)
#       include <GL/gl.h>
#    endif
#endif

#ifndef SGL_OPENGL_MAX_MAJOR
#   define SGL_OPENGL_MAX_MAJOR 4
#endif

#ifndef SGL_OPENGL_MAX_MINOR
#   define SGL_OPENGL_MAX_MINOR 1
#endif

#ifndef SGL_DEBUG
#    define SGL_DEBUG 0
#elif SGL_DEBUG >= 1
// Needed for printing SGL configuration info
#    include <iostream>
#endif


namespace sgl {
namespace config {
    bool sglOpenglVersion (int major, int minor);
} // end namespace

void sglInitialize (int major, int minor);

} // namespace


#ifndef SGL_USE_GLES
// OpenGL
#   define SGL_RENDERBUFFER_SUPPORTED     sgl::config::sglOpenglVersion(3,0)
#   define SGL_FRAMEBUFFER_SUPPORTED      sgl::config::sglOpenglVersion(3,0)
#   define SGL_VERTEXARRAY_SUPPORTED      sgl::config::sglOpenglVersion(3,0)
#   define SGL_UNIFORMBLOCK_SUPPORTED     sgl::config::sglOpenglVersion(3,3)
#   define SGL_PROGRAMPIPELINES_SUPPORTED sgl::config::sglOpenglVersion(4,1)
#   define SGL_TEXSTORAGE_SUPPORTED       sgl::config::sglOpenglVersion(4,2)
#   define SGL_COMPUTESHADER_SUPPORTED    sgl::config::sglOpenglVersion(4,3)
#   define SGL_DEBUGLOG_SUPPORTED         sgl::config::sglOpenglVersion(4,3)
#   define SGL_BUFFERSTORAGE_SUPPORTED    sgl::config::sglOpenglVersion(4,4)
#else
// OpenGL ES
#   define SGL_RENDERBUFFER_SUPPORTED     sgl::config::sglOpenglVersion(2,0)
#   define SGL_FRAMEBUFFER_SUPPORTED      sgl::config::sglOpenglVersion(2,0)
#   define SGL_VERTEXARRAY_SUPPORTED      sgl::config::sglOpenglVersion(2,0)
#   define SGL_UNIFORMBLOCK_SUPPORTED     sgl::config::sglOpenglVersion(3,0)
#   define SGL_TEXSTORAGE_SUPPORTED       sgl::config::sglOpenglVersion(3,0)
#   define SGL_PROGRAMPIPELINES_SUPPORTED sgl::config::sglOpenglVersion(3,1)
#   define SGL_COMPUTESHADER_SUPPORTED    sgl::config::sglOpenglVersion(3,1)
#   define SGL_BUFFERSTORAGE_SUPPORTED    false
#   define SGL_DEBUGLOG_SUPPORTED         sgl::config::sglOpenglVersion(3,2)
#endif

#define SGL_RW                         (SGL_BUFFERSTORAGE_SUPPORTED ? (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT) : GL_DYNAMIC_DRAW)



#endif // SGLCONFIG_H
