#include <SimpleGL/helpers/SimpleGLHelpers.h>
#include "sgl-test.h"

#include <glm/glm.hpp>
#include <vector>
#include <functional>

int main () {
    sgl::Context ctx(500,500,"dejong");


    sgl::Param<float> aval{"a", -2.7f, 0.01f};
    sgl::Param<float> bval{"b", -0.09f, 0.01f};
    sgl::Param<float> cval{"c", -0.86f, 0.01f};
    sgl::Param<float> dval{"d", -2.2f, 0.01f};
    //sgl::Param<glm::vec2> pos{"position", sgl::VecPrinter};
    glm::vec2 pos;
    bool recompute = true;

    ctx.addKeyHandler([
        &aval, &bval, &cval, &dval,&recompute](const sgl::KeyEvent& event){
        if (event.eventType != sgl::PRESS && event.eventType != sgl::REPEAT) return;
        if (event.key == sgl::KEY_A) event.shiftPressed ? aval.decrement() : aval.increment();
        else if (event.key == sgl::KEY_S) event.shiftPressed ? bval.decrement() : bval.increment();
        else if (event.key == sgl::KEY_D) event.shiftPressed ? cval.decrement() : cval.increment();
        else if (event.key == sgl::KEY_F) event.shiftPressed ? dval.decrement() : dval.increment();
        recompute = true;
    });

    ctx.addMouseHandler(sgl::SimpleDragger{[&pos,&recompute](const sgl::DragEvent& event) {
        pos = {event.mx, event.my};
        recompute = true;
    }});

    // Actual OpenGL stuff follows...

    sgl::Shader shader = sgl::loadShader(TEST_RES("dejong_vs.glsl"), TEST_RES("dejong_fs.glsl"));

    // TODO: Decide whether to store buffer size along with buffer.
    // See resource.h for discussion.
    size_t pointBufSize = 10000;
    sgl::ArrayBuffer<glm::vec2> pointBuf{NULL, pointBufSize, GL_STATIC_DRAW};
    sgl::VertexArray vao;
    sgl::vertexAttribBuilder(vao)
        .addBuffer<sgl::vec2f>(pointBuf)
        .commit();


    glEnable(GL_PROGRAM_POINT_SIZE);
    glViewport(0,0,ctx.attrs.width, ctx.attrs.height);

    while (ctx.isAlive()) {
        ctx.pollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        if (recompute) {
            float px = pos.x;
            float py = pos.y;
            auto bv  = sgl::buffer_view<glm::vec2>(pointBuf); // TODO: Fix template deduction.
            for (size_t i = 0; i < pointBufSize; i++){
                float x = std::sin(aval.get() * py) - std::cos(bval.get() * px);
                float y = std::sin(cval.get() * px) - std::cos(dval.get() * py);
                px = x;
                py = y;
                bv[i] = {x,y};
            }
            recompute = false;
        }

        shader.bind();
        vao.bind();
        glDrawArrays(GL_POINTS, 0, pointBufSize);

        ctx.swapBuffers();
        sglCatchGLError();
    }

}
