#include <SimpleGL/SimpleGL.h>
#include "sgl-test.h"
#include <array>
#include <iostream>


void main1 () {
    sgl::Context ctx{500,500,"pbo test"};
    sgl::MeshResource renderQuad = sgl::createPlane(1);

    std::array<char,500*500*3> data;

    sgl::Shader shader = sgl::loadShader(TEST_RES("ident_vs.glsl"), TEST_RES("texture_fs.glsl"));

    sglCatchGLError();

    GLuint tex;
    glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 500, 500, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    sglCatchGLError();


    GLuint pbos[2];
    glGenBuffers(2,pbos);
    for (int i = 0; i < 2; i++){
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos[i]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, data.size(), &data[0], GL_STREAM_DRAW);
    }
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    sglCatchGLError();

    int pboIdx = 0;
    int pboIdxNext = 0;

    int frame = 0;
    glViewport(0,0,500,500);

    while (ctx.isAlive()){
        ctx.pollEvents();

        frame = (frame + 1) % 255;
        for (size_t i = 0; i < data.size(); i++){
            data[i] = frame;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        pboIdx = (pboIdx + 1) % 2;
        pboIdxNext = (pboIdxNext + 1) % 2;

        glBindTexture(GL_TEXTURE_2D, tex);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos[pboIdx]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 500, 500, GL_RGB, GL_UNSIGNED_BYTE, 0);

        sglCatchGLError();

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbos[pboIdxNext]);
        void * dest = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, 500*500*3, GL_MAP_WRITE_BIT);
        memcpy(dest, &data[0], data.size());
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        sglCatchGLError();

        shader.bind();
        shader.setTexture("image", GL_TEXTURE_2D, tex, 0);

        sglCatchGLError();

        renderQuad.bind();

        sglCatchGLError();

        glDrawElements(GL_TRIANGLES, renderQuad.size, GL_UNSIGNED_INT, 0);

        sglCatchGLError();

        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        sglCatchGLError();

        ctx.swapBuffers();
    }

}

void main2 () {
    sgl::Context ctx{500,500,"pbo test"};
    sgl::MeshResource renderQuad = sgl::createPlane(1);

    std::array<char,500*500*3> data;
    sgl::Shader shader = sgl::loadShader(TEST_RES("ident_vs.glsl"), TEST_RES("texture_fs.glsl"));

    sgl::Texture2D tex = sgl::Texture2DBuilder()
        .build(ctx.attrs.width, ctx.attrs.height);

    sgl::PBOUploader uploader(tex);

    int frame = 0;
    glViewport(0,0,500,500);

    while (ctx.isAlive()){
        ctx.pollEvents();

        frame = (frame + 1) % 255;
        for (size_t i = 0; i < data.size(); i++){
            data[i] = frame;
        }
        uploader.update(data);

        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();
        shader.setTexture("image", tex, 0);

        renderQuad.bind();
        glDrawElements(GL_TRIANGLES, renderQuad.size, GL_UNSIGNED_INT, 0);

        ctx.swapBuffers();
    }
}

int main () {
    main1();
}
