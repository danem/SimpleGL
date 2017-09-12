#ifndef SGLCONFIG_H
#define SGLCONFIG_H

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

namespace sgl {
namespace detail {

} // namespace
} // namespace

#endif // SGLCONFIG_H
