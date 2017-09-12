#include <SimpleGL/utils.h>

using namespace sgl;
using namespace util;

std::string sgl::util::Formatter::str () const {
    return _stream.str();
}

sgl::util::Formatter::operator std::string () const {
    return _stream.str();
}

const char * sgl::util::glErrorToString (GLenum error){
    switch (error) {
        case GL_INVALID_ENUM: 
            return "GL_INVALID_ENUM: Invalid enumerant passed to function";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE: Numeric value out of accepted range";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION: Specified operation not allowed in current state";
        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW: Fuction call caused stack overflow";
        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW: Function call caused stack underflow";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY: There is not enough memory available to execute function call";
        default:
            return "No error";
    }
}
