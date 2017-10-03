# SimpleGL

SimpleGL is a very thin C++11 wrapper over OpenGL. Its aim is to make OpenGL development
faster and less error prone without sacraficing speed or the full range of features
OpenGL has to offer. SimpleGL can be seamlessly integrated into existing OpenGL applications
or used with other OpenGL libraries and frameworks.
```c++
#include <SimpleGL/SimpleGL.h>
#include <vector>
#include <glm/glm.hpp>

int main () {
    sgl::Context ctx{500,500,"instancing test"};
    sgl::Shader shader = sgl::loadShader("instanced_vs.glsl", "fs.glsl");

    std::vector<glm::vec3> offsets;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++){
            offsets.emplace_back(i/10.f, j/10.f, i/j);
        }
    }
    
    // Create a fixed size OpenGL GL_ARRAY_BUFFER with the contents of offsets.
    // sizeof(ArrayBuffer<glm::vec3>) == sizeof(GLuint)
    sgl::ArrayBuffer<glm::vec3> offsetsBuf{offsets};
    sgl::MeshResource mesh = sgl::createPlane();
    
    // Add new buffer to our VAO
    sgl::vertexAttribBuilder(mesh,mesh.attribs)
        .addBuffer(offsetsBuf)
        .commit();

    sgl::Texture2D texture = sgl::TextureBuilder2D()
        .build("image.png");

    glViewport(0, 0, ctx.attrs.width, ctx.attrs.height);
    while (ctx.isAlive()){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ctx.pollEvents();

        shader.bind();
        shader.setTexture("image", texture);

        mesh.bind();
        glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.size, offsets.size());

        ctx.swapBuffers();
    }
}
```

### Features:

* Zero overhead "typesafe" opengl object wrappers
* Opengl object lifetime managment
* Convenient shader compilation and access API
* Builtin performance counters and debug logs for < OpenGL 4.3 (eg: OSX)
* Low overhead, safe texture interface
* Basic parametric mesh primitive constructors
* Simple, flexible opengl context creation API
* C++11

### Dependencies

* GLEW or libepoxy
* GLFW 3+ (optional)
* GLM (optional)

### Compatibility

Tested on OSX and Ubuntu.
