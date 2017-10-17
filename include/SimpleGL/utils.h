#pragma once

#include "sglconfig.h"
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <map>
#include <vector>
#include <initializer_list>
#include <iostream>


namespace sgl {
namespace util {

const char * glErrorToString (GLenum error);

class Formatter {
private:
    std::stringstream _stream;

public:
    Formatter () {}
    ~Formatter() {}

    template <class T>
    Formatter& operator<< (const T& value) {
        _stream << value;
        return *this;
    }

    std::string str() const;
    operator std::string () const;
};


struct DebugConfigLevel {
    GLenum source;
    GLenum type;
    GLenum severity;
};

struct DebugConfig {
    std::vector<DebugConfigLevel> levels;

    DebugConfig () {}

    DebugConfig (const std::initializer_list<DebugConfigLevel>& opts) {
        for (const auto& l : opts) {
            levels.emplace_back(l);
        }
    }
};

#if SGL_DEBUG > 0
const DebugConfig SGL_DEBUG_MAX_VERBOSE{{GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE}};
const DebugConfig SGL_DEBUG_VERBOSE{
    {GL_DEBUG_SOURCE_API, GL_DONT_CARE, GL_DONT_CARE},
    {GL_DEBUG_SOURCE_SHADER_COMPILER, GL_DONT_CARE, GL_DONT_CARE}
};

const DebugConfig SGL_DEBUG_ERRORS_VERBOOSE {{GL_DONT_CARE, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE}};
const DebugConfig SGL_DEBUG_ERRORS {
    {GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE},
    {GL_DEBUG_SOURCE_SHADER_COMPILER, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE}
};

static inline void __sglDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                            GLsizei length, const GLchar* message, const void* userParam) {
    Formatter msg;

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:         msg << "ERROR: "; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       msg << "WARNING: "; break;
    case GL_DEBUG_SEVERITY_LOW:          msg << "INFO: "; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: msg << "LOG: "; break;
    default:                             msg << "UNKNOWN: "; break;
    }

    switch (source) {
    case GL_DEBUG_SOURCE_API:             msg << "OpenGL "; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   msg << "Window "; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     msg << "Third Party "; break;
    case GL_DEBUG_SOURCE_OTHER:           msg << "Other "; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: msg << "Shader Compiler "; break;
    default:                              msg << "Unknown "; break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:               msg << "Error "; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: msg << "Deprecated "; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  msg << "Undefined Behavior "; break;
    case GL_DEBUG_TYPE_PORTABILITY:         msg << "Portability "; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         msg << "Performance "; break;
    case GL_DEBUG_TYPE_MARKER:              msg << "Marker "; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          msg << "Push Group "; break;
    case GL_DEBUG_TYPE_POP_GROUP:           msg << "Pop Group "; break;
    case GL_DEBUG_TYPE_OTHER:               msg << "Other "; break;
    }

    msg << "id: " << id << " message: " << message;
    std::cout << msg.str() << std::endl;
}

static void initializeDebugging (const DebugConfig& config) {
    if (SGL_DEBUGLOG_SUPPORTED) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(__sglDebugMessageCallback, nullptr);
        for (const auto& l : config.levels) {
            glDebugMessageControl(l.source, l.type, l.severity, 0, NULL, GL_TRUE);
        }
    } else {
        printf("Warning: OpenGL debug log not supported\n");
    }
}
#endif
} // end namespace
} // end namespace


#define SGL_BOOL(v) ((v) ? GL_TRUE : GL_FALSE)

#define sglDbgPrint(...) do { printf("%s:%d ", __FILE__, __LINE__); printf(__VA_ARGS__); } while(0);

// Check GL error, save it to input variable,
// print error message, and optionally quit
#define sglGetGLError(ignore) do {\
       GLenum err = glGetError();\
       if (err != GL_NO_ERROR) {\
            const char * msg = sgl::util::glErrorToString(err);\
            printf("Error code %d at %s:%d: %s\n", err, __FILE__, __LINE__, msg);\
            if (!ignore) exit(EXIT_FAILURE);\
       }\
   } while (0);

#define sglClearGLError() glGetError();
#define sglCatchGLError() sglGetGLError(0);
#define sglCheckGLError() sglGetGLError(1);

#if SGL_DEBUG >= 1
#    define sglDbgLog(...) sglDbgPrint(__VA_ARGS__)
#    define sglDbgCatchGLError() sglCatchGLError()
#    define sglDbgCatchGLErrorLog(...) sglCatchGLErrorLog(__VA_ARGS__)
#else
#    define sglDbgLog(...)
#    define sglDbgCatchGLError()
#    define sglDbgCatchGLErrorLog(...)
#endif

#if SGL_DEBUG >= 2
#    define sglDbgLogVerbose(...) sglDbgPrint(__VA_ARGS__)
#    define sglDbgLogCreation(kind,len,dest) do { for(int i=0;i<len;i++){ sglDbgPrint("created resource %d:%d\n", kind,dest[i]); }} while(0);
#    define sglDbgLogDeletion(kind,len,dest) do { for(int i=0;i<len;i++){ sglDbgPrint("deleted resource %d:%d\n", kind,dest[i]); }} while(0);
#else
#    define sglDbgLogVerbose(...)
#    define sglDbgLogCreation(kind,len,dest)
#    define sglDbgLogDeletion(kind,len,dest)
#endif

#if SGL_DEBUG >= 3
#    define sglDbgLogVerbose2(...) sglDbgPrint(__VA_ARGS__)
#    define sglDbgLogBind(kind,dest) do { sglDbgPrint("bound resource %d:%d\n",kind,dest); } while (0);
#else
#    define sglDbgLogVerbose2(...)
#    define sglDbgLogBind(kind,dest)
#endif




