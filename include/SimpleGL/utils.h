#pragma once

#include "sglconfig.h"
#include <string>
#include <fstream>
#include <sstream>

#define _sglCatchGLError(ignore) do {\
       GLenum err = glGetError();\
       if (err != GL_NO_ERROR) {\
            const char * msg = sgl::glErrorToString(err);\
            printf("Error code %d at %s:%d: %s\n", err, __FILE__, __LINE__, msg);\
            if (!ignore) exit(EXIT_FAILURE);\
       }\
   } while (0);

#define sglCatchGLError() _sglCatchGLError(false)
#define sglCheckGLError() _sglCatchGLError(true)

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
