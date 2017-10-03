#ifndef SIMPLEGLHELPERS_H
#define SIMPLEGLHELPERS_H

#include <SimpleGL/SimpleGL.h>
#include "context.h"
#include "camera.h"
#include "event.h"
#include "mesh.h"
#include "param.h"
#include "pbo.h"
#include "transform.h"

namespace sgl {
namespace traits {
    // Needed to use glm with sgl traits.
    template<> struct GLType<glm::vec2> { const static GLenum type = GL_FLOAT; };
    template<> struct GLType<glm::vec3> { const static GLenum type = GL_FLOAT; };
    template<> struct GLType<glm::vec4> { const static GLenum type = GL_FLOAT; };
    template<> struct GLType<glm::mat3> { const static GLenum type = GL_FLOAT; };
    template<> struct GLType<glm::mat4> { const static GLenum type = GL_FLOAT; };
} // end namespace
} // end namespace


#endif // SIMPLEGLHELPERS_H
