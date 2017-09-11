#ifndef _SHADER_H_
#define _SHADER_H_

#include "sglconfig.h"
#include "traits.h"
#include "resource.h"

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

namespace sgl {

namespace detail {

    struct ShaderInputs {

        bool hasGeometryShader;
        bool isComputeShader;

        const std::string vertPath;
        const std::string vertSrc;

        const std::string fragPath;
        const std::string fragSrc;

        const std::string geomPath;
        const std::string geomSrc;

        const std::string computePath;
        const std::string computeSrc;

        ShaderInputs (
            const std::string& vp, const std::string& vs,
            const std::string& fp, const std::string& fs,
            const std::string& gp, const std::string& gs
        ) : 
            vertPath(vp), vertSrc(vs),
            fragPath(fp), fragSrc(fs),
            geomPath(gp), geomSrc(gs),
            hasGeometryShader(true), isComputeShader(false)
        {}

        ShaderInputs (
            const std::string& vp, const std::string& vs,
            const std::string& fp, const std::string& fs
        ) : 
            vertPath(vp), vertSrc(vs),
            fragPath(fp), fragSrc(fs),
            hasGeometryShader(false), isComputeShader(false)
        {}

        ShaderInputs ( const std::string& cp, const std::string& cs) :
            computePath(cp), computeSrc(cs), isComputeShader(true), hasGeometryShader(false)
        {}
    };

    GLuint compileShaderProgram (GLenum type, const std::string& path, const std::string& src);
    GLuint compileAndLinkProgram (const ShaderInputs& inputs);

} // namespace

// TODO: Make this a GLResource
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
        setTexture(id, kind, (GLuint)texture, unit);
    }

    GLint setTexture (const std::string& id, GLenum target, GLuint handle, int textureUnit);

};

Shader loadShader (const std::string& computePath);
Shader loadShader (const std::string& vertPath, const std::string& fragPath);
Shader loadShader (const std::string& vertPath, const std::string& fragPath, const std::string& geomPath);

Shader compileShader (const std::string& computeSrc);
Shader compileShader (const std::string& vertSrc, const std::string& fragSrc);
Shader compileShader (const std::string& vertSrc, const std::string& fragSrc, const std::string& geomSrc);

}
#endif
