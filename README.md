# SimpleGL

SimpleGL is a very thin C++11 wrapper over OpenGL. Its aim is to make OpenGL development
faster and less error prone without sacraficing speed or the full range of features
OpenGL has to offer. SimpleGL can be seamlessly integrated into existing OpenGL applications
or used with other OpenGL libraries and frameworks.

```c++
std::vector<float> data(1000);            //
std::iota(data.begin(), data.end(), 0);   //
                                          //
sgl::GLResource<GL_ARRAY_BUFFER> buffer1; // GLuint buffer1;
sgl::bufferData<float>(buffer1, data);    // glGenBuffers(1,&buffer1);
                                          // glBindBuffer(GL_ARRAY_BUFFER, buffer1);
                                          // glBufferData(GL_ARRAY_BUFFER, sizeof(float)*data.size(), data.data(), GL_DYNAMIC_DRAW);
                                          //
                                          //
buffer.bind();                            // GLResource can be used interchangably with GLuint
assert(sizeof(buffer) == sizeof(GLuint)); // 
glBindBuffer(buffer.type, buffer);        //
buffer.release();                         //
                                          //
                                          // We can introduce some type aliases to make things cleaner. 
                                          // All SGL Types are just variations on GLResource.
                                          //
sgl::ArrayBuffer<int> buffer2{data};      // GLuint buffer2;
                                          // glGenBuffers(1, &buffer2);
                                          // glBindBuffer(GL_ARRAY_BUFFER, buffer2);
                                          // glBufferStorage(GL_ARRAY_BUFFER, sizeof(float)*data.size(), data.data(), GL_MAP_READ_BIT);
                                          //
                                          // Now add a VAO
                                          //
sgl::VertexArray vao;                     // GLuint vao;
sgl::VertexAttribBuilder(vao)             // glGenVertexArrays(1,&vao);
    .addBuffer<float,float>(buffer2)      // glBindVertexArray(vao);
    .commit();                            // glBindBuffer(GL_ARRAY_BUFFER, buffer2);
                                          // glEnableVertexAttribArray(0);
                                          // glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
                                          // glEnableVertexAtrribArray(1);
                                          // glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)sizeof(float));
                                          // glBindVertexArray(0);
                                          //
                                          // 
Texture2D tex = TextureBuilder2D()        // GLuint tex;
    .format(GL_R, GL_R8)                  // glGenTextures(1,&tex);
    .build(100,100);                      // glBindTexture(GL_TEXTURE_2D, tex);
                                          // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                                          // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                                          // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                                          // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                                          // glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 100, 100, 0, GL_R, GL_UNSIGNED_BYTE, NULL);
                                          //
                                          //
{ auto res = resource_guard(buffer1); }   // Ensure a resource is freed at the end of a block
{ auto bg = bind_guard(buffer2); }        // Ensure a resource is unbound at the end of a block
                                          //
void withBuffer (ArrayBuffer<int>& buf);  // Typesafe interfaces
                                          //
                                          //
                                          // And much more!                          
```
Or using the helper library:                                       

```c++
#include <vector>
#include <glm/glm.hpp>

// Core SimpleGL Library
#include <SimpleGL/SimpleGL.h>

// Optional helper library containing camera class, meshes, etc...
#include <SimpleGL/helpers/SimpleGLHelpers.h>

using sgl::helper::Context;
using sgl::helper::MeshResource;

int main () {
    Context ctx{500,500,"instancing test"};
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
    MeshResource mesh = sgl::helper::createPlane();
    
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

* libepoxy
* GLFW 3+ (optional: needed for helper library and example projects)
* GLM (optional: needed for helper library and example projects)

### Compatibility

Tested on OSX and Ubuntu.
