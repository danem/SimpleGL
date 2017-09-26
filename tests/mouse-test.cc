#include <SimpleGL/SimpleGL.h>
#include "sgl-test.h"

#include <glm/glm.hpp>
#include <glm/gtx/type_trait.hpp>
#include <glm/detail/precision.hpp>

#include <iostream>
#include <type_traits>

template <class T>
void VecPrinter (const char * name, const T& value){
    int len = T::length();
    std::cout << name << ": {";
    for (int i = 0; i < len; i++){
        std::cout << value[i];
        if (i < len - 1) std::cout << ", ";
    }
    std::cout << "}" << std::endl;
}

class MouseDragger : public sgl::MouseDraggerBase {
public:

    sgl::Param<glm::vec2> mp;

    MouseDragger () :
        sgl::MouseDraggerBase(),
        mp("mouse", VecPrinter)
    {}

    void onDragStart () override {}
    void onDragEnd () override {}
    void onDrag (double ox, double oy, double mx, double my, double dx, double dy) override {
        mp.set({mx,my});
    }
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
