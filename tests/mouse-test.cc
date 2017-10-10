#include <SimpleGL/helpers/SimpleGLHelpers.h>
#include "sgl-test.h"

#include <glm/glm.hpp>
#include <glm/gtx/type_trait.hpp>
#include <glm/detail/precision.hpp>

#include <iostream>
#include <type_traits>

class MouseDragger : public sgl::MouseDraggerBase {
public:

    sgl::Param<glm::vec2> mp;

    MouseDragger () :
        sgl::MouseDraggerBase(),
        mp("mouse", sgl::VecPrinter)
    {}

    void onDragStart () override {}
    void onDragEnd () override {}
    void onDrag (const sgl::DragEvent& event) override {
        mp.set({event.mx,event.my});
    }

    void onScroll (double sx, double sy) override {}
};


int main () {
    sgl::Context ctx{500,500,"mouse test"};

    MouseDragger md;
    ctx.addMouseHandler(md);

    sgl::MeshResource renderQuad = sgl::createPlane();

    glViewport(0,0,ctx.attrs.width, ctx.attrs.height);
    while (ctx.isAlive()){
        glClear(GL_COLOR_BUFFER_BIT);
        ctx.pollEvents();
        ctx.swapBuffers();
    }
}
