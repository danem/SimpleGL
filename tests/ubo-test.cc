#include <SimpleGL/helpers/SimpleGLHelpers.h>
#include "sgl-test.h"

struct UBOData {
    int x;
    int y;
    int z;
};

int main () {
    sgl::Context ctx{500, 500, "ubo test"};

    UBOData data;
    sgl::UniformBuffer<UBOData> ubo{&data, 1};

    sgl::Shader uboShader = sgl::loadShader(TEST_RES("ubo_vs.glsl"), TEST_RES("ubo_fs.glsl"));

    sgl::MeshResource plane = sgl::createPlane();

    glViewport(0, 0, ctx.attrs.width, ctx.attrs.height);

    int frame = 0;
    while (ctx.isAlive()){
        ctx.pollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        {
            data.x = data.y = data.z = frame;
            auto bv = sgl::buffer_view<UBOData>(ubo, GL_WRITE_ONLY);
            bv = data;
            frame += 1;
        }

        auto sg = sgl::bind_guard(uboShader);
        auto mg = sgl::bind_guard(plane);

        ctx.swapBuffers();
    }
}
