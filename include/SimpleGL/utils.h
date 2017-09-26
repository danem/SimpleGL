#pragma once

#include "sglconfig.h"
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>

#define _sglGetGLError(ignore) do {\
       GLenum err = glGetError();\
       if (err != GL_NO_ERROR) {\
            const char * msg = sgl::util::glErrorToString(err);\
            printf("Error code %d at %s:%d: %s\n", err, __FILE__, __LINE__, msg);\
            if (!ignore) exit(EXIT_FAILURE);\
       }\
   } while (0);

#define sglCatchGLError() _sglGetGLError(false)
#define sglCheckGLError() _sglGetGLError(true)
#define sglClearGLError() glGetError()
#define SGL_BOOL(v) ((v) ? GL_TRUE : GL_FALSE)


#ifdef SGL_DEBUG
#    define sglDbgCatchGLError() _sglGetGLError(false)
#    define sglDgbLog(...) do { printf(__VA_ARGS__); } while(0);
#else
#    define sglDbgCatchGLError()
#    define sglDgbLog(...)
#endif

#if SGL_DEBUG >= 2
#    define sglDgbLogVerbose(...) sglDbgLog(__VA_ARGS__)
#    define sglDgbLogWarn(...) sglDgbLog(__VA_ARGS__)
#    define sglDbgLogInfo(...) sglDbgLog(__VA_ARGS__)
#elif SGL_DEBUG >= 1
#    define sglDgbLogVerbose(...)
#    define sglDgbLogWarn(...) sglDgbLog(__VA_ARGS__)
#    define sglDbgLogInfo(...) sglDbgLog(__VA_ARGS__)
#else
#    define sglDgbLogVerbose(...)
#    define sglDgbLogWarn(...)
#    define sglDbgLogInfo(...)
#endif

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

template <class T>
class Range {
public:
    T low;
    T high;

    Range (T high) : high(high), low(0){}
    Range (T high, T low) : high(high), low(low) {}
};

} // end namespace
} // end namespace
