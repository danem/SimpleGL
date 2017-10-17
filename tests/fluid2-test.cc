#define SGL_DEBUG 2
#include <SimpleGL/helpers/SimpleGLHelpers.h>
#include "sgl-test.h"
#include <string>
#include <glm/glm.hpp>
#include <algorithm>
#include <random>
#include <vector>
#include <chrono>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "contrib/stb_image.h"

using random_bytes_engine = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char>;


void linkShader (sgl::Shader shader, sgl::VertexShader vs, const std::string& type, const std::string& src) {
    std::string ver = "#version 330 core\n";
    sgl::FragmentShader fs = sgl::compileShaderStage<GL_FRAGMENT_SHADER>({ver, "#define ", type, "\n", src}, type);
    sgl::linkShaderStages(shader,vs,fs);
    fs.release();
}

inline sgl::Slab2D createSlab2D (const sgl::TextureBuilder2D& builder, int w, int h){
    sgl::Texture2D a = builder.build(w,h);
    sgl::Texture2D b = builder.build(w,h);
    return {a,b};
}


void clearColor (sgl::Framebuffer& fbo, float r, float g, float b, float a) {
    auto bg = sgl::bind_guard(fbo);
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0,0,0,1);
}

void clearColor (sgl::Framebuffer& fbo, float r) {
    clearColor(fbo,r,r,r,1);
}

void clearSlab (sgl::Slab2D& slab, float r, float g, float b, float a){
    clearColor(slab.ping().fbo, r, g, b, a);
    clearColor(slab.pong().fbo, r, g, b, a);
}

void clearSlab (sgl::Slab2D& slab, float v){
    clearSlab(slab, v, v, v, 1);
}

struct ShaderParams {
    glm::vec4 emitterColor;
    glm::vec2 emitterPosition;
    float emitterRadius;
    float timeStep;
    float dissipation;
    float width;
    float height;
    float sign;

    ShaderParams (float w, float h) :
        emitterColor(1,0,0,1),
        emitterPosition(1.0f,1.0f),
        emitterRadius(0.003f),
        timeStep(0.01f),
        dissipation(1.f),
        width(w), height(h),
        sign(1)
    {}
};

struct SimState {
    sgl::MeshResource renderQuad;

    sgl::Shader advectShader;
    sgl::Shader emitterShader;
    sgl::Shader vizShader;
    sgl::Shader convolveShader;

    SimState () :
        renderQuad(sgl::createPlane())
    {
        sgl::VertexShader vs = sgl::loadShaderStage<GL_VERTEX_SHADER>(TEST_RES("ident_vs.glsl"));
        std::string fragSrc = sgl::loadShaderSource(TEST_RES("fluid2_fs.glsl"));

        linkShader(emitterShader, vs, "EMITTER_SHADER", fragSrc);
        linkShader(advectShader, vs, "ADVECT_SHADER", fragSrc);
        linkShader(vizShader, vs, "VISUALIZE_SHADER", fragSrc);
        linkShader(convolveShader, vs, "CONVOLVE_SHADER", fragSrc);
        vs.release();
    }
};

void visualize (SimState& state, sgl::Surface2D& image) {
    sgl::bind<GL_FRAMEBUFFER>(0);
    glClear(GL_COLOR_BUFFER_BIT);
    state.vizShader.bind();
    state.vizShader.setTexture("Source", image.texture, 0);
    state.renderQuad.bind();
    glDrawElements(GL_TRIANGLES, state.renderQuad.size, GL_UNSIGNED_INT, 0);
    sglCatchGLError();
}

void applyAdvect (SimState& state, sgl::Surface2D& vel, sgl::Surface2D& source, sgl::Surface2D& dest, sgl::UniformBuffer<ShaderParams>& params) {
    auto bg = sgl::bind_guard(dest.fbo);
    state.advectShader.bind();
    state.advectShader.setTexture("Velocity", vel.texture, 0);
    state.advectShader.setTexture("Source", source.texture, 0);
    state.advectShader.setUniformBlock("ShaderParams", params, 0);
    state.renderQuad.bind();
    glDrawElements(GL_TRIANGLES, state.renderQuad.size, GL_UNSIGNED_INT, 0);
    sglCatchGLError();
}

void applyEmitter (SimState& state, sgl::Surface2D& dest, sgl::UniformBuffer<ShaderParams>& params){
    auto bg = sgl::bind_guard(dest.fbo);
    state.emitterShader.bind();
    state.emitterShader.setUniformBlock("ShaderParams", params, 0);
    state.renderQuad.bind();
    glDrawElements(GL_TRIANGLES, state.renderQuad.size, GL_UNSIGNED_INT, 0);
    sglCatchGLError();
}

void applyConvolve (SimState& state, sgl::Surface2D source, sgl::Surface2D& dest, sgl::UniformBuffer<ShaderParams>& params, float center, float corner, float edge) {
    float weights [] = {corner, edge, corner,
                        edge, center, edge,
                        corner, edge, corner};
    auto bg = sgl::bind_guard(dest.fbo);
    state.convolveShader.bind();
    state.convolveShader.setTexture("Source", source.texture, 0);
    state.convolveShader.setUniformBlock("ShaderParams", params, 0);
    state.convolveShader.setUniformMatrix3f("weights", weights);
    state.renderQuad.bind();
    glDrawElements(GL_TRIANGLES, state.renderQuad.size, GL_UNSIGNED_INT, 0);
    sglCatchGLError();
}


int main () {
    sgl::Context ctx{500,500,"fluid"};

    SimState simState;

    ShaderParams params(ctx.attrs.width, ctx.attrs.height);
    sgl::UniformBuffer<ShaderParams> paramsBuf{&params,1};

    sgl::TextureAccessor loader{stbi_load, stbi_image_free};

    sgl::TextureBuilder2D texBuilder;
    sgl::Slab2D density  = createSlab2D(texBuilder, ctx.attrs.width, ctx.attrs.height);
    sgl::Slab2D velocity = createSlab2D(texBuilder, ctx.attrs.width, ctx.attrs.height);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    sgl::Texture2D image = sgl::TextureBuilder2D().build(TEST_RES("sun-ra.jpg"), loader);

    const char * slabNames[] = {
        "density", "velocity"
    };

    sgl::Slab2D slabs[] = {
        density,
        velocity
    };

    sgl::Param<int> currentSlab{"current slab", &sgl::ListPrinter<int,char*>::print, slabNames};
    sgl::Param<float&> emitterRadius("emitter radius", params.emitterRadius, 0.01f);
    sgl::Param<float&> timeStep("time step", params.timeStep, 0.01f);
    sgl::Param<int> renderStage("render stage");
    sgl::Param<float&> signParam("sign param", params.sign, 1);
    bool stateChanged = false;

    ctx.addKeyHandler([
        &stateChanged, &emitterRadius, &timeStep, &signParam,
        &currentSlab, &renderStage, &ctx](const sgl::KeyEvent& event){
        if (event.eventType != sgl::PRESS && event.eventType != sgl::REPEAT) return;
        if (event.key == sgl::KEY_R) event.shiftPressed ? emitterRadius.decrement() : emitterRadius.increment();
        else if (event.key == sgl::KEY_T) event.shiftPressed ? timeStep.decrement() : timeStep.increment();
        else if (event.key == sgl::KEY_S) currentSlab.set((currentSlab.get() + 1) % 2);
        else if (event.key == sgl::KEY_Q) renderStage.set((renderStage.get() + 1) % 5);
        else if (event.key == sgl::KEY_F) signParam.set(signParam.get() * -1);
        else if (event.key == sgl::KEY_ESCAPE) ctx.close();
        stateChanged = true;
    });

    ctx.addMouseHandler(sgl::SimpleDragger([&stateChanged, &params](const sgl::DragEvent& event){
        stateChanged = true;
        float x = static_cast<float>(event.mx) / 500;
        float y = 1 - (static_cast<float>(event.my) / 500);
        params.emitterPosition = {x,y};
    }));

    sglDbgCatchGLError();

    random_bytes_engine rbe;
    std::vector<unsigned char> data(velocity.ping().texture.attrs.size());
    std::generate(begin(data), end(data), std::ref(rbe));

    clearSlab(density, 0);
    sgl::updateTexture(velocity.ping().texture, data.data(), 0, 0);

    {
        auto bg = sgl::bind_guard(density.ping().fbo);
        simState.vizShader.bind();
        simState.vizShader.setTexture("Source", image, 0);
        simState.renderQuad.bind();
        glDrawElements(GL_TRIANGLES, simState.renderQuad.size, GL_UNSIGNED_INT, 0);
        sglCatchGLError();
    }

    for (int i = 0; i < 3; i++){
        applyConvolve(simState, velocity.ping(), velocity.pong(), paramsBuf, 1, 0.111f, 0.333f);
        velocity.swap();
    }

    applyConvolve(simState, velocity.ping(), velocity.pong(), paramsBuf, 8, -1, -1);
    velocity.swap();

    glViewport(0, 0, ctx.attrs.width, ctx.attrs.height);

    int frame = 0;

    while (ctx.isAlive()){
        ctx.pollEvents();

        if (renderStage == 0) { goto end; }

        if (stateChanged) {
            sgl::bufferData(paramsBuf, &params, 1);
            stateChanged = false;
        }


        //applyEmitter(simState, velocity.ping(), paramsBuf);
        //applyEmitter(simState, density.ping(), paramsBuf);

        if (renderStage == 1) { goto end; }

        for (int i = 0; i < 4; i++) {
            applyAdvect(simState, velocity.ping(), density.ping(), density.pong(), paramsBuf);
            density.swap();
        }

        if (frame % 10 == 0) {
            params.sign *= -1;
            stateChanged = true;
        }


end:
        visualize(simState, slabs[currentSlab].ping());
        ctx.swapBuffers();
        sglCatchGLError();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        frame += 1;
    }
}
