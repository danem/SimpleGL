#include <SimpleGL/shader.h>

using namespace sgl;

Shader sgl::loadShader (const std::string& computePath) {
    auto stage = resource_guard(loadShaderStage<GL_COMPUTE_SHADER>(computePath));
    Shader shader;
    linkShaderStages(shader, stage.get());
    return shader;
}

Shader sgl::loadShader (const std::string& vertPath, const std::string& fragPath) {
    auto vs = resource_guard(loadShaderStage<GL_VERTEX_SHADER>(vertPath));
    auto fs = resource_guard(loadShaderStage<GL_FRAGMENT_SHADER>(fragPath));
    Shader shader;
    linkShaderStages(shader, vs.get(), fs.get());
    return shader;
}

Shader sgl::loadShader (const std::string& vertPath, const std::string& fragPath, const std::string& geomPath) {
    auto vs = resource_guard(loadShaderStage<GL_VERTEX_SHADER>(vertPath));
    auto fs = resource_guard(loadShaderStage<GL_FRAGMENT_SHADER>(fragPath));
    auto gs = resource_guard(loadShaderStage<GL_GEOMETRY_SHADER>(geomPath));
    Shader shader;
    linkShaderStages(shader, vs.get(),fs.get(),gs.get());
    return shader;
}

Shader sgl::compileShader (const std::string& computeSrc) {
    auto stage = resource_guard(compileShaderStage<GL_COMPUTE_SHADER>(computeSrc));
    Shader shader;
    linkShaderStages(shader, stage.get());
    return shader;
}

Shader sgl::compileShader (const std::string& vertSrc, const std::string& fragSrc) {
    auto vs = resource_guard(compileShaderStage<GL_VERTEX_SHADER>(vertSrc));
    auto fs = resource_guard(compileShaderStage<GL_FRAGMENT_SHADER>(fragSrc));
    Shader shader;
    linkShaderStages(shader, vs.get(),fs.get());
    return shader;
}

Shader sgl::compileShader (const std::string& vertSrc, const std::string& fragSrc, const std::string& geomSrc) {
    auto vs = resource_guard(compileShaderStage<GL_VERTEX_SHADER>(vertSrc));
    auto fs = resource_guard(compileShaderStage<GL_FRAGMENT_SHADER>(fragSrc));
    auto gs = resource_guard(compileShaderStage<GL_GEOMETRY_SHADER>(geomSrc));
    Shader shader;
    linkShaderStages(shader, vs.get(),fs.get(),gs.get());
    return shader;
}

GLint Shader::getLocation (const char * id) {
    GLint loc = glGetUniformLocation(_id, id);
    if (loc == -1) return loc;
    return loc;
}


GLint Shader::setUniformMatrix4f (std::string& id, float * matrix){
    int loc = glGetUniformLocation(_id, id.c_str());
    if (loc == -1) return loc;
    glUniformMatrix4fv(loc, 1, false, matrix);
    return loc;
}

GLint Shader::setUniformMatrix4f (const char * id, float * matrix){
    int loc = glGetUniformLocation(_id, id);
    if (loc == -1) return loc;
    glUniformMatrix4fv(loc, 1, false, matrix);
    return loc;
}

GLint Shader::setUniformMatrix2f (std::string& id, float * matrix){
    int loc = glGetUniformLocation(_id, id.c_str());
    if (loc == -1) return loc;
    glUniformMatrix2fv(loc, 1, false, matrix);
    return loc;
}

GLint Shader::setUniformMatrix2f (const char * id, float * matrix){
    int loc = glGetUniformLocation(_id, id);
    if (loc == -1) return loc;
    glUniformMatrix2fv(loc, 1, false, matrix);
    return loc;
}


GLint Shader::setUniform1f (const char * id, float v) {
    int loc = glGetUniformLocation(_id, id);
    if (loc == -1) return loc;
    glUniform1f(loc, v);
    return loc;
}

GLint Shader::setUniform2fv (std::string& id, float * vector){
    int loc = glGetUniformLocation(_id, id.c_str());
    if (loc == -1) return loc;
    glUniform2fv(loc,1,vector);
    return loc;
}

GLint Shader::setUniform2fv (const char * id, float * vector){
    int loc = glGetUniformLocation(_id, id);
    if (loc == -1) return loc;
    glUniform2fv(loc,1,vector);
    return loc;
}

GLint Shader::setUniform2fv (const char * id, float x, float y){
    int loc = glGetUniformLocation(_id, id);
    if (loc == -1) return loc;
    float temp[2] = {x,y};
    glUniform2fv(loc,1,temp);
    return loc;
}

GLint Shader::setUniform3fv (std::string& id, float * vector){
    int loc = glGetUniformLocation(_id, id.c_str());
    if (loc == -1) return loc;
    glUniform3fv(loc,1,vector);
    return loc;
}

GLint Shader::setUniform3fv (const char * id, float * vector){
    int loc = glGetUniformLocation(_id, id);
    if (loc == -1) return loc;
    glUniform3fv(loc,1,vector);
    return loc;
}

GLint Shader::setUniform3fv (const char * id, float x, float y, float z) {
    int loc = glGetUniformLocation(_id, id);
    if (loc == -1) return loc;
    float temp[3] = {x,y,z};
    glUniform3fv(loc,1,temp);
    return loc;
}

GLint Shader::setUniform4fv (const char * id, float * vec) {
    int loc = glGetUniformLocation(_id,id);
    if (loc == -1) return loc;
    glUniform4fv(loc,1,vec);
    return loc;
}

GLint Shader::setUniform4fv (std::string& id, float * vec) {
    int loc = glGetUniformLocation(_id,id.c_str());
    if (loc == -1) return loc;
    glUniform4fv(loc,1,vec);
    return loc;
}

GLint Shader::setUniform4fv (const char * id, float x, float y, float z, float w) {
    int loc = glGetUniformLocation(_id,id);
    if (loc == -1) return loc;
    float temp [4] {x,y,z,w};
    glUniform4fv(loc,1,temp);
    return loc;
}

GLint Shader::setUniformBool (const char * id, bool v){
    int loc = glGetUniformLocation(_id, id);
    if (loc == -1) return loc;
    glUniform1i(loc, v ? 1 : 0);
    return loc;
}

GLint Shader::setTexture (const std::string& id, GLenum target, GLuint handle, int textureUnit){
    int loc = glGetUniformLocation(_id,id.c_str());
    if (loc == -1) return loc;
    glUniform1i(loc, textureUnit);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(target, handle);
    return loc;
}

GLint Shader::setUniformBlock(const char * id, GLResource<GL_UNIFORM_BUFFER>& buffer, GLuint unit) {
    unsigned int idx = glGetUniformBlockIndex(_id,id);
    if (idx == GL_INVALID_INDEX) return idx;
    glBindBufferBase(GL_UNIFORM_BUFFER, unit, static_cast<GLuint>(buffer));
    glUniformBlockBinding(_id, idx, unit);
    return idx;
}
