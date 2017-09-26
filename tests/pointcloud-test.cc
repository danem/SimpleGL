#include <SimpleGL/SimpleGL.h>
#include "sgl-test.h"
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include <time.h>
#include <math.h>

float randomFloat (float hi, float lo){
    return lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));
}

void generatePointCloud (size_t count, std::vector<glm::vec4>& pts){
    for (int i = 0; i < count; i++){
        pts.emplace_back(randomFloat(1,-1), randomFloat(1,-1), randomFloat(1,-1), randomFloat(0,5));
    }
}

int main () {
    sgl::Context ctx{500, 500, "point cloud"};
    sglClearGLError();

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    sgl::PerspectiveCamera cam{ctx.attrs.width, ctx.attrs.height};
    cam.position = {0,3,5};
    cam.looking = {0,0,0};
    cam.update();

    sgl::CameraController cc(ctx.attrs.width, ctx.attrs.height, cam);
    ctx.addMouseHandler(cc);

    sgl::Shader pcShader = sgl::loadShader(TEST_RES("pointcloud_vs.glsl"),TEST_RES("pointcloud_fs.glsl"));

    std::vector<glm::vec4> pts;
    generatePointCloud(500,pts);

    sgl::ArrayBuffer<glm::vec4> pcb{pts};

    sgl::VertexArray vao;
    sgl::vertexAttribBuilder(vao)
        .add<glm::vec3, float>(pcb);
    
    glViewport(0,0, ctx.attrs.width, ctx.attrs.height); 
    glClearColor(0,0,0,0);

    while (ctx.isAlive()){
        ctx.pollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pcShader.bind();
        pcShader.setUniformMatrix4f("cameraMat", cam.getProjection());
        pcShader.setUniformMatrix4f("viewMat", cam.getView());

        auto bg = sgl::bind_guard(vao);
        glDrawArrays(GL_POINTS, 0, pts.size());

        ctx.swapBuffers();
        sglCatchGLError();
    }
}
