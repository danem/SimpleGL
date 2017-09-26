#include <SimpleGL/shader.h>
#include <SimpleGL/utils.h>
#include <sstream>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <iostream>

using namespace sgl;

GLuint sgl::detail::compileShaderProgram (GLenum type, const std::string& path, const std::string& src) {
    int shader = glCreateShader(type);
    const char * source = src.c_str();

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint res;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
    if (res == GL_FALSE){
        char msgbuf[200] = {0};
        GLsizei len = 0;
        glGetShaderInfoLog(shader, 200, &len, msgbuf);
        int err = glGetError();
        std::stringstream errMsg;
        errMsg << "Got error code " << err << " while compiling: " << path << " \n" << msgbuf;
        throw std::runtime_error(errMsg.str());
    }
    return shader;
}

GLuint sgl::detail::compileAndLinkProgram (const sgl::detail::ShaderInputs& inputs) {
    GLuint prog = glCreateProgram();
    std::vector<GLuint> shaders;

    if (inputs.isComputeShader) {
        shaders.push_back(sgl::detail::compileShaderProgram(GL_COMPUTE_SHADER, inputs.computePath, inputs.computeSrc));
    } else {
        shaders.push_back(sgl::detail::compileShaderProgram(GL_VERTEX_SHADER, inputs.vertPath, inputs.vertSrc));
        shaders.push_back(sgl::detail::compileShaderProgram(GL_FRAGMENT_SHADER, inputs.fragPath, inputs.fragSrc));
        if (inputs.hasGeometryShader) {
            shaders.push_back(sgl::detail::compileShaderProgram(GL_GEOMETRY_SHADER, inputs.geomPath, inputs.geomSrc));
        }
    }

    for (const auto h : shaders) {
        glAttachShader(prog, h);
    }

    glLinkProgram(prog);

    for (const auto h : shaders) {
        glDeleteShader(h);
    }

    GLint res;
    glGetProgramiv(prog, GL_LINK_STATUS, &res);
    if (res == GL_FALSE){
        char msgbuf[200] = {0};
        GLsizei len = 0;
        glGetProgramInfoLog(prog, 200, &len, msgbuf);
        int err = glGetError();
        std::stringstream errMsg;
        errMsg << "Got error code " << err << ":\n" << msgbuf;
        throw std::runtime_error(errMsg.str());
    }

    return prog;
}

sgl::Shader sgl::loadShader (const std::string& computePath) {
    std::ifstream cf(computePath);
    std::string cSrc((std::istreambuf_iterator<char>(cf)), std::istreambuf_iterator<char>());
    sgl::detail::ShaderInputs is(computePath, cSrc);
    return { sgl::detail::compileAndLinkProgram(is) };
}

sgl::Shader sgl::loadShader (const std::string& vertPath, const std::string& fragPath) {
    std::ifstream vf(vertPath);
    std::string vertSrc((std::istreambuf_iterator<char>(vf)),
                         std::istreambuf_iterator<char>());

    std::ifstream ff(fragPath);
    std::string fragSrc((std::istreambuf_iterator<char>(ff)),
                         std::istreambuf_iterator<char>());

    sgl::detail::ShaderInputs is(vertPath, vertSrc, fragPath, fragSrc);

    return { sgl::detail::compileAndLinkProgram(is) };
}

sgl::Shader sgl::loadShader (const std::string& vertPath, const std::string& fragPath, const std::string& geomPath) {
    std::ifstream vf(vertPath);
    std::string vertSrc((std::istreambuf_iterator<char>(vf)),
                         std::istreambuf_iterator<char>());

    std::ifstream ff(fragPath);
    std::string fragSrc((std::istreambuf_iterator<char>(ff)),
                         std::istreambuf_iterator<char>());

    std::ifstream gf(geomPath);
    std::string geomSrc((std::istreambuf_iterator<char>(gf)),
                         std::istreambuf_iterator<char>());

    sgl::detail::ShaderInputs is(vertPath, vertSrc, fragPath, fragSrc, geomPath, geomSrc);

    return { sgl::detail::compileAndLinkProgram(is) };
}


sgl::Shader sgl::compileShader (const std::string& computeSrc) {
    sgl::detail::ShaderInputs is("", computeSrc);
    return { sgl::detail::compileAndLinkProgram(is) };
}

sgl::Shader sgl::compileShader (const std::string& vertSrc, const std::string& fragSrc) {
    sgl::detail::ShaderInputs is("", vertSrc, "", fragSrc);
    return { sgl::detail::compileAndLinkProgram(is) };
}

sgl::Shader sgl::compileShader (const std::string& vertSrc, const std::string& fragSrc, const std::string& geomSrc) {
    sgl::detail::ShaderInputs is("", vertSrc, "", fragSrc, "", geomSrc);
    return { sgl::detail::compileAndLinkProgram(is) };
}

GLint sgl::Shader::getLocation (const char * id) {
    GLint loc = glGetUniformLocation(_id, id);
    if (loc == GL_INVALID_VALUE) throw std::runtime_error("Invalid shader program");
    else if (loc == GL_INVALID_OPERATION) throw std::runtime_error("Supplied object is not a shader program");
    else if (loc == GL_INVALID_OPERATION) throw std::runtime_error("Shader program not successfully linked");
    return loc;
}


GLint Shader::setUniformMatrix4f (std::string& id, float * matrix){
    int loc = glGetUniformLocation(_id, id.c_str());
    glUniformMatrix4fv(loc, 1, false, matrix);
    return loc;
}

GLint Shader::setUniformMatrix4f (const char * id, float * matrix){
    int loc = glGetUniformLocation(_id, id);
    glUniformMatrix4fv(loc, 1, false, matrix);
    return loc;
}

GLint Shader::setUniformMatrix2f (std::string& id, float * matrix){
    int loc = glGetUniformLocation(_id, id.c_str());
    glUniformMatrix2fv(loc, 1, false, matrix);
    return loc;
}

GLint Shader::setUniformMatrix2f (const char * id, float * matrix){
    int loc = glGetUniformLocation(_id, id);
    glUniformMatrix2fv(loc, 1, false, matrix);
    return loc;
}


GLint Shader::setUniform1f (const char * id, float v) {
    int loc = glGetUniformLocation(_id, id);
    glUniform1f(loc, v);
    return loc;
}

GLint Shader::setUniform2fv (std::string& id, float * vector){
    int loc = glGetUniformLocation(_id, id.c_str());
    glUniform2fv(loc,1,vector);
    return loc;
}

GLint Shader::setUniform2fv (const char * id, float * vector){
    int loc = glGetUniformLocation(_id, id);
    glUniform2fv(loc,1,vector);
    return loc;
}

GLint Shader::setUniform2fv (const char * id, float x, float y){
    int loc = glGetUniformLocation(_id, id);
    float temp[2] = {x,y};
    glUniform2fv(loc,1,temp);
    return loc;
}

GLint Shader::setUniform3fv (std::string& id, float * vector){
    int loc = glGetUniformLocation(_id, id.c_str());
    glUniform3fv(loc,1,vector);
    return loc;
}

GLint Shader::setUniform3fv (const char * id, float * vector){
    int loc = glGetUniformLocation(_id, id);
    glUniform3fv(loc,1,vector);
    return loc;
}

GLint Shader::setUniform3fv (const char * id, float x, float y, float z) {
    int loc = glGetUniformLocation(_id, id);
    float temp[3] = {x,y,z};
    glUniform3fv(loc,1,temp);
    return loc;
}

GLint Shader::setUniform4fv (const char * id, float * vec) {
    int loc = glGetUniformLocation(_id,id);
    glUniform4fv(loc,1,vec);
    return loc;
}

GLint Shader::setUniform4fv (std::string& id, float * vec) {
    int loc = glGetUniformLocation(_id,id.c_str());
    glUniform4fv(loc,1,vec);
    return loc;
}

GLint Shader::setUniform4fv (const char * id, float x, float y, float z, float w) {
    int loc = glGetUniformLocation(_id,id);
    float temp [4] {x,y,z,w};
    glUniform4fv(loc,1,temp);
    return loc;
}

GLint Shader::setUniformBool (const char * id, bool v){
    int loc = glGetUniformLocation(_id, id);
    glUniform1i(loc, v ? 1 : 0);
    return loc;
}

GLint Shader::setTexture (const std::string& id, GLenum target, GLuint handle, int textureUnit){
    int loc = glGetUniformLocation(_id,id.c_str());
    glUniform1i(loc, textureUnit);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(target, handle);
    return loc;
}

void Shader::setUniformBlock(const char * id, GLResource<GL_UNIFORM_BUFFER>& buffer, GLuint unit) {
    unsigned int idx = glGetUniformBlockIndex(_id,id);
    glBindBufferBase(GL_UNIFORM_BUFFER, unit, static_cast<GLuint>(buffer));
    glUniformBlockBinding(_id, idx, unit);
}

//
//GLint Shader::setTexture (std::string id, Texture * texture, int unit) {
//    return setTexture(id, texture->getTarget(), texture->getHandle(), unit);
//}
//
//GLint Shader::setTexture (std::string id, Texture& texture, int unit){
//    return setTexture(id, texture.getTarget(), texture.getHandle(), unit);
//}







