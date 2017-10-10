#define SGL_DEBUG 2
#include <SimpleGL/helpers/SimpleGLHelpers.h>
#include "sgl-test.h"

#include <vector>
#include <random>
#include <climits>
#include <algorithm>
#include <functional>
#include <chrono>
#include <thread>

using random_bytes_engine = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char>;


void clearColor (sgl::Framebuffer& fbo, float r, float g, float b, float a){
    auto bg = sgl::bind_guard(fbo);
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0,0,0,1);
}

void clearColor (sgl::Framebuffer& fbo, float v){
    clearColor(fbo, v, v, v, 1.f);
}

sgl::Surface2D createSurface (int w, int h, int components) {
    GLenum fmts[4] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};
    GLenum ifmts[4] = {GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F};

    sgl::Texture2D tex = sgl::TextureBuilder2D()
            .format(fmts[components-1], ifmts[components-1])
            .dataType(GL_FLOAT)
            .build(w,h);

    sgl::Surface2D surf{tex};
    clearColor(surf.fbo, 0, 0, 0, 1);
    return surf;
}

sgl::Slab2D createSlab (int w, int h, int components) {
    return {createSurface(w,h,components), createSurface(w,h,components)};
}

struct SimState {
    sgl::Framebuffer fbo;

    size_t width;
    size_t height;
    size_t gridWidth;
    size_t gridHeight;
    float timeStep;
    float cellSize;
    float splatRadius;
    float smokeBouyancy;
    float smokeWeight;
    float ambientTemperature;
    float impulseTemperature;
    float temperatureDissipation;
    float velocityDissipation;
    float densityDissipation;
    float gradientScale;
    float impulseDensity;
    float obstacleRadius;
    float noiseWeight;
    glm::vec2 obstaclePosition;
    int numJacobiIterations;
    glm::vec2 position;

    sgl::Slab2D velocity;
    sgl::Slab2D density;
    sgl::Slab2D pressure;
    sgl::Slab2D temperature;
    sgl::Surface2D divergence;
    sgl::Surface2D obstacles;
    sgl::Slab2D noise;

    // Ensure we don't double allocate.
    // See note in resource.h
    sgl::Shader subtractShader;
    sgl::Shader jacobiShader;
    sgl::Shader advectShader;
    sgl::Shader impulseShader;
    sgl::Shader divergenceShader;
    sgl::Shader bouyancyShader;
    sgl::Shader obstacleShader;
    sgl::Shader textureShader;

    sgl::MeshResource renderQuad;

    SimState (size_t w, size_t h) :
        width(w), height(h),
        gridWidth(w),
        gridHeight(h),
        renderQuad(sgl::createPlane()),
        velocity(createSlab(w,h,2)),
        density(createSlab(w,h,1)),
        pressure(createSlab(w,h,1)),
        temperature(createSlab(w,h,1)),
        divergence(createSurface(w,h,3)),
        obstacles(createSurface(w,h,3)),
        noise(createSlab(w,h,2)),
        noiseWeight(0.00001f),
        timeStep(0.01f),
        cellSize(1.25f),
        smokeBouyancy(1.0f),
        smokeWeight(0.05f),
        gradientScale(1.125f / cellSize),
        temperatureDissipation(0.99f),
        ambientTemperature(1.0f),
        velocityDissipation(0.99f),
        densityDissipation(0.9999f),
        splatRadius(gridWidth / 8.0f),
        impulseDensity(2),
        impulseTemperature(10.f),
        obstacleRadius(0.2),
        obstaclePosition(0.5f,0.5f),
        numJacobiIterations(10),
        position(gridWidth / 2, gridWidth / 2)

    {
        // More efficient than loading the same file and recompiling the vertex shader for
        // each shader that we have.
        std::string fragSource = sgl::loadShaderSource(TEST_RES("fluid_fs.glsl"));
        sgl::VertexShader identShader = sgl::loadShaderStage<GL_VERTEX_SHADER>(TEST_RES("ident_vs.glsl"));
        linkShader(subtractShader  , identShader, fragSource, "SUBTRACT_SHADER");
        linkShader(jacobiShader    , identShader, fragSource, "JACOBI_SHADER");
        linkShader(advectShader    , identShader, fragSource, "ADVECT_SHADER");
        linkShader(impulseShader   , identShader, fragSource, "IMPULSE_SHADER");
        linkShader(divergenceShader, identShader, fragSource, "DIVERGENCE_SHADER");
        linkShader(bouyancyShader  , identShader, fragSource, "BOUYANCY_SHADER");
        linkShader(obstacleShader  , identShader, fragSource, "OBSTACLE_SHADER");
        linkShader(textureShader   , identShader, fragSource, "VISUALIZE_SHADER");
    }

    void release () {
        subtractShader.release();
        jacobiShader.release();
        advectShader.release();
        impulseShader.release();
        divergenceShader.release();
        bouyancyShader.release();
        obstacleShader.release();
        textureShader.release();
        velocity.release();
        density.release();
        temperature.release();
        pressure.release();
        obstacles.release();
        divergence.release();
        renderQuad.release();
        fbo.release();
    }

private:
    void linkShader (sgl::Shader& shader, sgl::VertexShader& vs, const std::string& src, const std::string& type){
        const std::string& ver = "#version 330 core\n";
        sgl::FragmentShader frag = sgl::compileShaderStage<GL_FRAGMENT_SHADER>({ver, "#define ", type, "\n", src}, type);
        sgl::linkShaderStages(shader, vs, frag);
    }
};

template <GLenum kind>
void visualize (SimState& state, sgl::GLResource<kind>& texture){
    static_assert(sgl::traits::IsTex2D<kind>::value, "Texture must be a texture2D instance");
    glViewport(0, 0, state.width, state.height);
    sgl::bind<GL_FRAMEBUFFER>(0);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    state.textureShader.bind();
    state.textureShader.setTexture("image", texture, 0);
    state.textureShader.setUniform3fv("fillColor", 1, 1, 1);
    state.textureShader.setUniform2fv("scale", 1.f / state.width, 1.f / state.height);
    glDrawElements(GL_TRIANGLES, state.renderQuad.size, GL_UNSIGNED_INT, 0);

    glDisable(GL_BLEND);
    sglDbgCatchGLError();
}

void initObstacles (SimState& state, sgl::Surface2D& dest) {
    auto bg = sgl::bind_guard(dest.fbo);

    state.obstacleShader.bind();
    state.obstacleShader.setUniform2fv("position", state.obstaclePosition.x, state.obstaclePosition.y);
    state.obstacleShader.setUniform4fv("color", 1, 0, 0, 0);
    state.obstacleShader.setUniform1f("radius", state.obstacleRadius);

    state.renderQuad.bind();
    glDrawElements(GL_TRIANGLES, state.renderQuad.size, GL_UNSIGNED_INT, 0);
    sglDbgCatchGLError();
}

void applyAdvect (
    SimState& state,
    sgl::Surface2D& velocity, sgl::Surface2D& source,
    sgl::Surface2D& obstacles, sgl::Surface2D& dest,
    float dissipation
) {
    auto bg = sgl::bind_guard(dest.fbo);

    state.advectShader.bind();
    state.advectShader.setTexture("VelocityTexture", velocity.texture, 0);
    state.advectShader.setTexture("SourceTexture", source.texture, 1);
    state.advectShader.setTexture("Obstacles", obstacles.texture, 2);
    state.advectShader.setUniform2fv("InverseSize", 1.f / state.width, 1.f / state.height);
    state.advectShader.setUniform1f("TimeStep", state.timeStep);
    state.advectShader.setUniform1f("Dissipation", dissipation);

    state.renderQuad.bind();
    glDrawElements(GL_TRIANGLES, state.renderQuad.size, GL_UNSIGNED_INT, 0);
    sglDbgCatchGLError();
}

void applyBouyancy (
    SimState& state,
    sgl::Surface2D& velocity, sgl::Surface2D& temperature,
    sgl::Surface2D& density, sgl::Surface2D& dest
) {
    auto bg = sgl::bind_guard(dest.fbo);

    state.bouyancyShader.bind();
    state.bouyancyShader.setTexture("velocityTexture", velocity.texture, 0);
    state.bouyancyShader.setTexture("tempTexture", temperature.texture, 1);
    state.bouyancyShader.setTexture("densityTexture", density.texture, 2);
    state.bouyancyShader.setUniform1f("ambientTemperature", state.ambientTemperature);
    state.bouyancyShader.setUniform1f("timeStep", state.timeStep);
    state.bouyancyShader.setUniform1f("sigma", state.smokeBouyancy);
    state.bouyancyShader.setUniform1f("kappa", state.smokeWeight);

    state.renderQuad.bind();
    glDrawElements(GL_TRIANGLES, state.renderQuad.size, GL_UNSIGNED_INT, 0);
    sglDbgCatchGLError();
}

void applyDivergence (
    SimState& state,
    sgl::Surface2D& velocity, sgl::Surface2D& obstacle,
    sgl::Surface2D& dest
) {
    auto bg = sgl::bind_guard(dest.fbo);

    state.divergenceShader.bind();
    state.divergenceShader.setTexture("Velocity", velocity.texture, 0);
    state.divergenceShader.setTexture("Obstacles", obstacle.texture, 1);
    state.divergenceShader.setUniform1f("HalfInverseCellSize", 0.5f / state.cellSize);

    state.renderQuad.bind();
    glDrawElements(GL_TRIANGLES, state.renderQuad.size, GL_UNSIGNED_INT, 0);
    sglDbgCatchGLError();
}

void applyImpulse (
    SimState& state,
    sgl::Surface2D& dest,
    glm::vec2& pos, float value, float radExtra = 0
) {
    auto bg = sgl::bind_guard(dest.fbo);

    state.impulseShader.bind();
    state.impulseShader.setUniform1f("radius", state.splatRadius + radExtra);
    state.impulseShader.setUniform2fv("point", pos.x, pos.y);
    state.impulseShader.setUniform3fv("fillColor", value, value, value);

    state.renderQuad.bind();
    glDrawElements(GL_TRIANGLES, state.renderQuad.size, GL_UNSIGNED_INT, 0);
    sglDbgCatchGLError();
}

void applyJacobi (
    SimState& state,
    sgl::Surface2D& pressure, sgl::Surface2D& divergence,
    sgl::Surface2D& obstacle, sgl::Surface2D& dest
) {
    auto bg = sgl::bind_guard(dest.fbo);

    state.jacobiShader.bind();
    state.jacobiShader.setTexture("Pressure", pressure.texture, 0);
    state.jacobiShader.setTexture("Divergence", divergence.texture, 1);
    state.jacobiShader.setTexture("Obstacles", obstacle.texture, 2);
    state.jacobiShader.setUniform1f("Alpha", -state.cellSize * state.cellSize);
    state.jacobiShader.setUniform1f("InverseBeta", 0.25f);

    state.renderQuad.bind();
    glDrawElements(GL_TRIANGLES, state.renderQuad.size, GL_UNSIGNED_INT, 0);
    sglDbgCatchGLError();
}

void applySubtract (
    SimState& state,
    sgl::Surface2D& velocity, sgl::Surface2D& pressure,
    sgl::Surface2D& obstacle, sgl::Surface2D& dest
) {
    auto bg = sgl::bind_guard(dest.fbo);

    state.subtractShader.bind();
    state.subtractShader.setTexture("Velocity", velocity.texture, 0);
    state.subtractShader.setTexture("Pressure", pressure.texture, 1);
    state.subtractShader.setTexture("Obstacles", obstacle.texture, 2);
    state.subtractShader.setUniform1f("GradientScale", state.gradientScale);

    state.renderQuad.bind();
    glDrawElements(GL_TRIANGLES, state.renderQuad.size, GL_UNSIGNED_INT, 0);
    sglDbgCatchGLError();
}


struct Dragger : public sgl::MouseDraggerBase {
    SimState& state;

    Dragger(SimState& state) :
        state(state)
    {}

    void onDragStart () override {}
    void onDragEnd () override {}
    void onDrag (const sgl::DragEvent& event) override {
        state.position = {event.mx,state.height-event.my};
    }

    void onScroll (double sx, double sy) override {}
};


int main () {
    sgl::Context ctx(500,900,"fluid sim");
    SimState state(ctx.attrs.width, ctx.attrs.height);

    const char * slabNames [] = {
        "velocity", "temperature",
        "density", "pressure", "divergence",
        "noise"
    };

    sgl::Surface2D slabs[] = {
        state.velocity.ping(),
        state.temperature.ping(),
        state.density.ping(),
        state.pressure.ping(),
        state.divergence,
        state.noise.ping()
    };


    sgl::Param<int> renderStage{"render stage"};
    sgl::Param<int> currentSlab{"current slab", &sgl::ListPrinter<int,char*>::print, slabNames};
    sgl::Param<float&> timeStep{"time step",state.timeStep, 0.01f};
    sgl::Param<float&> ambientTemp{"ambient temperature", state.ambientTemperature, 0.01f};
    sgl::Param<float&> tempDisp {"temperature dissipation", state.temperatureDissipation, 0.01f};
    sgl::Param<float&> densityDisp {"density dissipation", state.densityDissipation, 0.01f};
    sgl::Param<float&> impulseTemp{"impulse temperature", state.impulseTemperature, 0.1f};
    sgl::Param<float&> impulseDensity {"impulse density", state.impulseDensity, 0.1f};
    sgl::Param<int&> jacobiIters{"jacobi iterations", state.numJacobiIterations, 1};
    sgl::Param<float&> splatRadius{"splat radius", state.splatRadius, 1.f};
    sgl::Param<float&> gradientScale{"gradient scale", state.gradientScale, 0.01f};
    sgl::Param<float&> smokeWeight{"smoke weight", state.smokeWeight, 0.01f};
    sgl::Param<float&> smokeBouyancy{"smoke bouyancy", state.smokeBouyancy, 0.01f};
    sgl::Param<float&> velocityDisp {"velociy dissipation", state.velocityDissipation, 0.01f};
    sgl::Param<float&> cellSize{"cell size", state.cellSize, 0.01f};
    sgl::Param<bool> advectDensity{"advect density", true};
    sgl::Param<bool> advectTemperature{"advect temperature", true};
    sgl::Param<bool> advectVelocity{"advect velocity", true};
    bool continueRendering = true;

    renderStage.set(4);
    currentSlab.set(1);
    advectTemperature.set(true);
    advectDensity.set(true);
    advectVelocity.set(true);

    Dragger dragger{state};
    ctx.addMouseHandler(dragger);
    ctx.addKeyHandler([
        &currentSlab, &timeStep, &ambientTemp,
        &impulseTemp, &jacobiIters, &splatRadius,
        &renderStage,&gradientScale,&smokeWeight,
        &velocityDisp, &impulseDensity, &tempDisp,
        &smokeBouyancy, &densityDisp, &cellSize,
        &continueRendering, &advectDensity, &advectTemperature,
        &advectVelocity](const sgl::KeyEvent& evt) {
        if (evt.eventType != sgl::RELEASE && evt.eventType != sgl::REPEAT) return;
        if (evt.key == sgl::KEY_A) currentSlab.set((currentSlab + 1) % 6);
        else if (evt.key == sgl::KEY_Q) renderStage.set((renderStage + 1) % 5);
        else if (evt.key == sgl::KEY_Z) evt.shiftPressed ? timeStep.decrement() : timeStep.increment();
        else if (evt.key == sgl::KEY_M) evt.shiftPressed ? ambientTemp.decrement() : ambientTemp.increment();
        else if (evt.key == sgl::KEY_C) evt.shiftPressed ? impulseTemp.decrement() : impulseTemp.increment();
        else if (evt.key == sgl::KEY_J) evt.shiftPressed ? jacobiIters.decrement() : jacobiIters.increment();
        else if (evt.key == sgl::KEY_R) evt.shiftPressed ? splatRadius.decrement() : splatRadius.increment();
        else if (evt.key == sgl::KEY_G) evt.shiftPressed ? gradientScale.decrement() : gradientScale.increment();
        else if (evt.key == sgl::KEY_W) evt.shiftPressed ? smokeWeight.decrement() : smokeWeight.increment();
        else if (evt.key == sgl::KEY_V) evt.shiftPressed ? velocityDisp.decrement() : velocityDisp.increment();
        else if (evt.key == sgl::KEY_D) evt.shiftPressed ? impulseDensity.decrement() : impulseDensity.increment();
        else if (evt.key == sgl::KEY_T) evt.shiftPressed ? tempDisp.decrement() : tempDisp.increment();
        else if (evt.key == sgl::KEY_B) evt.shiftPressed ? smokeBouyancy.decrement() : smokeBouyancy.increment();
        else if (evt.key == sgl::KEY_L) evt.shiftPressed ? densityDisp.decrement() : densityDisp.increment();
        else if (evt.key == sgl::KEY_S) evt.shiftPressed ? cellSize.decrement() : cellSize.increment();
        else if (evt.key == sgl::KEY_ESCAPE) continueRendering = false;
        else if (evt.key == sgl::KEY_1) advectDensity.set(!advectDensity.get());
        else if (evt.key == sgl::KEY_2) advectVelocity.set(!advectVelocity.get());
        else if (evt.key == sgl::KEY_3) advectTemperature.set(!advectTemperature.get());
    });

    random_bytes_engine rbe;
    std::vector<unsigned char> data(state.noise.ping().texture.attrs.size());
    std::generate(begin(data), end(data), std::ref(rbe));

    //sgl::updateTexture(state.velocity.ping().texture, data.data(), 0, 0);
    //sgl::updateTexture(state.temperature.ping().texture, data.data(), 0, 0);
    //sgl::updateTexture(state.density.ping().texture, data.data(), 0, 0);
    sgl::updateTexture(state.noise.ping().texture, data.data(), 0, 0);

    //clearColor(state.density.ping().fbo, 1, 0, 0, 1);
    clearColor(state.temperature.ping().fbo, state.ambientTemperature);

    initObstacles(state, state.obstacles);

    glViewport(0,0,state.width, state.height);

    while (ctx.isAlive()){
        ctx.pollEvents();
        if (advectVelocity) {
            applyAdvect(state,
                state.velocity.ping(), state.velocity.ping(),
                state.obstacles, state.velocity.pong(),
                state.velocityDissipation
            );
            state.velocity.swap();
        }

        if (advectTemperature){
            applyAdvect(
                state,
                state.velocity.ping(), state.temperature.ping(),
                state.obstacles, state.temperature.pong(),
                state.temperatureDissipation
            );
            state.temperature.swap();
        }


        if (advectDensity) {
            applyAdvect(
                state,
                state.velocity.ping(), state.density.ping(),
                state.obstacles, state.density.pong(),
                state.densityDissipation
            );
            state.density.swap();
        }

        applyBouyancy(
            state,
            state.velocity.ping(), state.temperature.ping(),
            state.density.ping(), state.velocity.pong()
        );
        state.velocity.swap();


        if (renderStage == 1) { visualize(state, slabs[currentSlab].texture); goto end;}

        applyImpulse(state, state.temperature.ping(), state.position, state.impulseTemperature);
        applyImpulse(state, state.density.ping(), state.position, state.impulseDensity);
        applyDivergence(state, state.velocity.ping(), state.obstacles, state.divergence);

        if (renderStage == 2) { visualize(state, slabs[currentSlab].texture); goto end;}

        clearColor(state.pressure.ping().fbo, 0, 0, 0, 0);

        for (int i = 0; i < state.numJacobiIterations; i++){
            applyJacobi(state, state.pressure.ping(), state.divergence, state.obstacles, state.pressure.pong());
            state.pressure.swap();
        }

        if (renderStage == 3) { visualize(state, slabs[currentSlab].texture); goto end;}

        applySubtract(
            state,
            state.velocity.ping(), state.pressure.ping(),
            state.obstacles, state.velocity.pong()
        );
        state.velocity.swap();


        visualize(state, slabs[currentSlab].texture);
end:
        ctx.swapBuffers();
    }
}
