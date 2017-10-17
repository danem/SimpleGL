#include <SimpleGL/helpers/SimpleGLHelpers.h>
#include "sgl-test.h"
#include <array>
#include <time.h>
#include <random>
#include <chrono>
#include <thread>


float randomFloat (float hi, float lo){
    return lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));
}

struct Surface {
    sgl::Framebuffer fbo;
    sgl::Texture2D texture;

    Surface (sgl::Texture2D& texture) :
        texture(texture)
    {
        fbo.attachTexture(texture);
    }
};

struct Slab {
private:
    Surface _surfA;
    Surface _surfB;
    bool _useSurfA;

public:
    Slab (sgl::Texture2D& a, sgl::Texture2D& b) :
        _surfA(a),
        _surfB(b),
        _useSurfA(true)
    {}

    Surface& ping () {
        if (_useSurfA) return _surfA;
        else return _surfB;
    }

    Surface& pong () {
        if (_useSurfA) return _surfB;
        else return _surfA;
    }

    void swap () {
        _useSurfA = !_useSurfA;
    }
};

struct SimState {
    sgl::Context ctx;
    sgl::Shader cellShader;
    sgl::Shader shader;
    sgl::MeshResource renderQuad;
    bool continueSim;

    SimState (int width, int height) :
        ctx(width,height,"test"),
        cellShader(sgl::loadShader(TEST_RES("ident_vs.glsl"), TEST_RES("game-of-life_fs.glsl"))),
        shader(sgl::loadShader(TEST_RES("ident_vs.glsl"), TEST_RES("texture_fs.glsl"))),
        continueSim(true),
        renderQuad(sgl::createPlane(1))

    {}

};

void renderGOL (SimState& state, Slab& slab){ 
    while (state.ctx.isAlive() && state.continueSim) {
        state.ctx.pollEvents();
        Surface& current = slab.ping();
        {
            auto bg = sgl::bind_guard(current.fbo);

            state.cellShader.bind();
            state.cellShader.setTexture("image", slab.pong().texture, 0);

            state.renderQuad.bind();
            glDrawElements(GL_TRIANGLES, state.renderQuad.size, GL_UNSIGNED_INT, 0);
        }
        slab.swap();

        glClear(GL_COLOR_BUFFER_BIT);
        state.shader.bind();
        state.shader.setTexture("image", current.texture, 0);

        state.renderQuad.bind();
        glDrawElements(GL_TRIANGLES, state.renderQuad.size, GL_UNSIGNED_INT, 0);

        state.ctx.swapBuffers();
        sglCatchGLError();
    }
}

int main () {
    const int width = 500;
    const int height = 500;
    std::array<uint8_t,width*height*3> texData{};

    srand(time(NULL));


    SimState state(width,height);


    while (state.ctx.isAlive()) {
        state.continueSim = true;

        for (int i = 0; i < 300*100; i++) {
            int x = randomFloat(width-1,0);
            int y = randomFloat(height-1,0);
            texData[(y * width + x) * 3] = 255;
        }

        sgl::Texture2D texA = sgl::TextureBuilder2D()
            .build(width,height);

        sgl::Texture2D texB = sgl::TextureBuilder2D()
            .build(&texData[0], width, height);

        Slab slab(texA,texB);

        renderGOL(state, slab);

        texA.release();
        texB.release();
    }
}
