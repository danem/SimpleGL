# SimpleGL

SimpleGL is a very thin C++ wrapper over OpenGL. Its aim is to make OpenGL development
faster and less error prone without sacraficing speed or the full range of features
OpenGL has to offer. SimpleGL can be seamlessly integrated into existing OpenGL applications
or used with other OpenGL libraries and frameworks.

    #include <SimpleGL/SimpleGL.h>
    #include <vector>
    #include <glm/glm.hpp>

    int main () {
        sgl::Context ctx{500,500,"instancing test"};
        sgl::Shader shader = sgl::loadShader("instanced_vs.glsl", "fs.glsl");

        std::vector<glm::vec2> offsets;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++){
                offsets.emplace_back(i/10.f, j/10.f);
            }
        }
        sgl::ArrayBuffer<glm::vec2> offsetsBuf{offsets};
        sgl::MeshResource mesh = sgl::createPlane();

        sgl::VertexAttribBuilder(mesh,3,1)
            .add(offsetsBuf);

        glViewport(0, 0, ctx.attrs.width, ctx.attrs.height);
        while (ctx.isAlive()){
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            ctx.pollEvents();

            shader.bind();
            mesh.bind();
            glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.size, offsets.size());

            ctx.swapBuffers();
        }
    }





###Features:

* Near zero overhead "typesafe" opengl object wrappers
* Opengl object lifetime managment
* Convenient shader compilation and access API
* Low overhead, safe texture interface
* Basic parametric mesh primitive constructors
* Simple, flexible opengl context creation API


###Dependencies

* GLFW 3+
* GLEW
* GLM

