#ifndef SHADER_H
#define SHADER_H

#include "sglconfig.h"
#include "resource.h"

#include <string.h>
#include <vector>
#include <initializer_list>

namespace sgl{

/**
* ShaderStage represents a single compiled shader part (eg GL_VERTEX_SHADER).
* ShaderStages can be cached and relinked to save compilation time.
*/

template <GLenum kind>
struct ShaderStage : public GLResource<kind> {
    static_assert(traits::IsShaderStage<kind>::value, "Invalid shader stage type");
};


using VertexShader   = ShaderStage<GL_VERTEX_SHADER>;
using FragmentShader = ShaderStage<GL_FRAGMENT_SHADER>;
using GeometryShader = ShaderStage<GL_GEOMETRY_SHADER>;
using ComputShader   = ShaderStage<GL_COMPUTE_SHADER>;

/**
* Given a source string and an optional path, allocate a ShaderStage and
* compile the provided program. path is present for helpful error messages.
*/

template<GLenum kind>
ShaderStage<kind> compileShaderStage (const char ** source, size_t len, const std::string& path = "") {
    ShaderStage<kind> shader;
    glShaderSource(shader, len, source, NULL);
    glCompileShader(shader);

    GLint res;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
    if (res == GL_FALSE){
        char msgbuf[200] = {0};
        GLsizei len = 0;
        glGetShaderInfoLog(shader, 200, &len, msgbuf);
        GLenum err = glGetError();
        //std::stringstream errMsg;
        sglCatchGLError();
        //errMsg << "Got error code " << err << " while compiling: " << path << " \n" << msgbuf;
        //throw std::runtime_error(errMsg.str());
    }
    return shader;
}

template<GLenum kind>
ShaderStage<kind> compileShaderStage (const std::string& source, const std::string& path ="") {
    const char * src = source.c_str();
    return compileShaderStage<kind>(&src, 1, path);
}

template<GLenum kind>
ShaderStage<kind> compileShaderStage (const std::initializer_list<std::string>& source, const std::string& path ="") {
    std::vector<const char *> strings;
    for (const auto& s : source) strings.emplace_back(s.c_str());
    return compileShaderStage<kind>(strings.data(), strings.size(), path);
}


template<GLenum kind>
ShaderStage<kind> compileShaderStage (const std::vector<std::string>& source, const std::string& path ="") {
    std::vector<const char *> strings;
    for (const auto& s : source) strings.emplace_back(s.c_str());
    return compileShaderStage<kind>(strings.data(), strings.size(), path);
}

template<GLenum kind>
ShaderStage<kind> compileShaderStage (const std::vector<const char *>& source, const std::string& path ="") {
    return compileShaderStage<kind>(source.data(), source.size(), path);
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

template <GLenum kind>
ShaderStage<kind> loadShaderStage (const std::initializer_list<std::string>& paths, const std::string& name) {
    std::vector<std::string> files;
    for (const auto& p : paths) {
        std::ifstream vf(p);
        std::string src((std::istreambuf_iterator<char>(vf)),
                         std::istreambuf_iterator<char>());
        files.emplace_back(src);
    }
    return compileShaderStage<kind>(files, name);
}

template <GLenum kind>
ShaderStage<kind> loadShaderStage (const std::vector<std::string>& paths, const std::string& name) {
    std::vector<std::string> files;
    for (const auto& p : paths) {
        std::ifstream vf(p);
        std::string src((std::istreambuf_iterator<char>(vf)),
                         std::istreambuf_iterator<char>());
        files.emplace_back(src);
    }
    return compileShaderStage<kind>(files, name);
}

template <GLenum kind, size_t len>
ShaderStage<kind> loadShaderStage (const std::array<std::string,len>& paths, const std::string& name) {
    std::vector<std::string> files;
    for (const auto& p : paths) {
        std::ifstream vf(p);
        std::string src((std::istreambuf_iterator<char>(vf)),
                         std::istreambuf_iterator<char>());
        files.emplace_back(src);
    }
    return compileShaderStage<kind>(files, name);
}

inline std::string loadShaderSource (const std::string& path) {
    std::ifstream vf(path);
    std::string src((std::istreambuf_iterator<char>(vf)),
                    std::istreambuf_iterator<char>());
    vf.close();
    return src;
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
    GLint setTexture (const std::string& id, sgl::GLResource<kind>& texture, int unit=0) {
        static_assert(traits::IsTexture<kind>::value, "Must supply texture target");
        return setTexture(id, kind, (GLuint)texture, unit);
    }

    GLint setTexture (const std::string& id, GLenum target, GLuint handle, int textureUnit);

    GLint setUniformBlock (const char * id, GLResource<GL_UNIFORM_BUFFER>& ubo, GLuint unit = 0);

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
void linkShaderStages (Shader& shader, T& stage, Ts ...stages) {
    detail::linkShaderStagesHelper(shader,stage,stages...);
    glLinkProgram(shader);
    detail::catchShaderLinkErrors(shader);
}

template <class T>
void linkShaderStages (Shader& shader, T& stage) {
    detail::linkShaderStagesHelper(shader,stage);
    glLinkProgram(shader);
    detail::catchShaderLinkErrors(shader);
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
