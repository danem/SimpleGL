#ifndef SHADER_H
#define SHADER_H

#include "sglconfig.h"
#include "resource.h"

#include <string.h>

namespace sgl {

/**
* ShaderStage represents a single compiled shader part (eg GL_VERTEX_SHADER).
* ShaderStages can be cached and relinked to save compilation time.
*/
template <GLenum kind>
struct ShaderStage : GLResource<kind> {
    static_assert(traits::IsShaderStage<kind>::value, "Invalid shader stage type");
};

/**
* Given a source string and an optional path, allocate a ShaderStage and
* compile the provided program. path is present for helpful error messages.
*/
template<GLenum kind>
ShaderStage<kind> compileShaderStage (const std::string& source, const std::string& path = "") {
    ShaderStage<kind> shader;
    const char * src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint res;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
    if (res == GL_FALSE){
        char msgbuf[200] = {0};
        GLsizei len = 0;
        glGetShaderInfoLog(shader, 200, &len, msgbuf);
        GLenum err = glGetError();
        std::stringstream errMsg;
        errMsg << "Got error code " << err << " while compiling: " << path << " \n" << msgbuf;
        throw std::runtime_error(errMsg.str());
    }
    return shader;
}

/**
* Load a glsl program from the specified file and compile it.
*/
template <GLenum kind>
ShaderStage<kind> loadShaderStage (const std::string& path) {
    std::ifstream vf(path);
    std::string src((std::istreambuf_iterator<char>(vf)),
                     std::istreambuf_iterator<char>());
    return compileShaderStage<kind>(src,path);
}



class Shader : public GLResource<GL_PROGRAM> {
private:
    GLint getLocation (const char * id);

public:

    Shader () :
        GLResource<GL_PROGRAM>()
    {}

    Shader (GLuint handle) :
        GLResource<GL_PROGRAM>(handle)
    {}

    GLint setUniformMatrix4f (std::string& id, float * matrix);
    GLint setUniformMatrix2f (std::string& id, float * matrix);
    GLint setUniform2fv (std::string& id, float * vec);
    GLint setUniform3fv (std::string& id, float * vec);
    GLint setUniform4fv (std::string& id, float * vec);

    GLint setUniformMatrix4f (const char * id, float * matrix);
    GLint setUniformMatrix2f (const char * id, float * matrix);

    GLint setUniform2fv (const char * id, float * vec);
    GLint setUniform2fv (const char * id, float x, float y);

    GLint setUniform3fv (const char * id, float * vec);
    GLint setUniform3fv (const char * id, float x, float y, float z);

    GLint setUniform4fv (const char * id, float * vec);
    GLint setUniform4fv (const char * id, float x, float y, float z, float w);

    GLint setUniform1f (const char * id, float v);

    GLint setUniformBool (const char * id, bool v);

    template <GLenum kind>
    GLint setTexture (const std::string& id, sgl::GLResource<kind>& texture, int unit) {
        static_assert(traits::IsTexture<kind>::value, "Must supply texture target");
        return setTexture(id, kind, (GLuint)texture, unit);
    }

    GLint setTexture (const std::string& id, GLenum target, GLuint handle, int textureUnit);

    void setUniformBlock (const char * id, GLResource<GL_UNIFORM_BUFFER>& ubo, GLuint unit = 0);

};

namespace detail {
    inline void catchShaderLinkErrors (GLuint shader) {
        GLint res;
        glGetProgramiv(shader, GL_LINK_STATUS, &res);
        if (res == GL_FALSE){
            char msgbuf[200] = {0};
            GLsizei len = 0;
            glGetProgramInfoLog(shader, 200, &len, msgbuf);
            int err = glGetError();
            std::stringstream errMsg;
            errMsg << "Got error code " << err << ":\n" << msgbuf;
            throw std::runtime_error(errMsg.str());
        }
    }

    template <class T>
    void linkShaderStagesHelper (Shader& shader, T& stage) {
        static_assert(traits::IsShaderStage<T::type>::value, "Input must be shader stage");
        glAttachShader(shader,stage);
    }

    template <class T, class ...Ts>
    void linkShaderStagesHelper (Shader& shader, T& stage, Ts ...args){
        linkShaderStagesHelper(shader,stage);
        linkShaderStagesHelper(shader,args...);
    }

} // end namespace


// TODO: Having the signature like this kills IDEs code completion.
// T Should be a ShaderStage ...
template <class T, class ...Ts>
Shader linkShaderStages (T& stage, Ts ...stages) {
    Shader shader;
    detail::linkShaderStagesHelper(shader,stage,stages...);
    glLinkProgram(shader);
    detail::catchShaderLinkErrors(shader);
    return shader;
}

template <class T>
Shader linkShaderStages (T& stage) {
    Shader shader;
    detail::linkShaderStagesHelper(shader,stage);
    glLinkProgram(shader);
    detail::catchShaderLinkErrors(shader);
    return shader;
}

// Dead simple interface
Shader loadShader (const std::string& computePath);
Shader loadShader (const std::string& vertPath, const std::string& fragPath);
Shader loadShader (const std::string& vertPath, const std::string& fragPath, const std::string& geomPath);

Shader compileShader (const std::string& computeSrc);
Shader compileShader (const std::string& vertSrc, const std::string& fragSrc);
Shader compileShader (const std::string& vertSrc, const std::string& fragSrc, const std::string& geomSrc);


} // end of namespace

#endif // SHADER2_H
