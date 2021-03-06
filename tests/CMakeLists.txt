cmake_minimum_required(VERSION 3.2)
project(SGLTests)

set(CMAKE_CXX_STANDARD 11)

set(CONTRIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/contrib)

set(SHADER_FILES
    ./data/dejong_fs.glsl
    ./data/dejong_vs.glsl
    ./data/fluid_fs.glsl
    ./data/fluid2_fs.glsl
    ./data/fs.glsl
    ./data/game-of-life_fs.glsl
    ./data/ident_vs.glsl
    ./data/instanced_vs.glsl
    ./data/pointcloud_fs.glsl
    ./data/pointcloud_vs.glsl
    ./data/shader-test-header.glsl
    ./data/shader-test-vs.glsl
    ./data/texture_fs.glsl
    ./data/vs.glsl)


macro(test_target name filename)
    add_executable(${name} ${filename} ${SHADER_FILES})
    add_definitions(${DEFINITIONS})
    target_link_libraries(${name} PRIVATE SimpleGLHelpers)
    target_include_directories(${name} PRIVATE simpleglhelpers ${CURRENT_SOURCE_DIR})
endmacro(test_target)

set(DEFINITIONS ${DEFINITIONS}
    -DSGL_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

test_target(allocation-test  allocation-test.cc)
test_target(context-test     context-test.cc)
test_target(debug-test       debug-test.cc)
test_target(dejong-test      dejong-test.cc)
test_target(framebuffer-test framebuffer-test.cc)
test_target(game-of-life     game-of-life.cc)
test_target(instanced-test   instanced-test.cpp)
test_target(key-test         key-test.cc)
test_target(mouse-test       mouse-test.cc)
test_target(overhead-test    overhead-test.cc)
test_target(param-test       param-test.cc)
test_target(pbo-test         pbo-test.cc)
test_target(plane-test       plane-test.cc)
test_target(pointcloud-test  pointcloud-test.cc)
test_target(resource-test    resource-test.cc)
test_target(shader-test      shader-test.cc)
test_target(size-test        size-test.cc)
test_target(ubo-test         ubo-test.cc)
test_target(fluid-test       fluid-test.cc)
#test_target(fluid2-test      fluid2-test.cc)
test_target(texture-test     texture-test.cc)
test_target(vertex-test      vertex-test.cc)
test_target(traits-test      traits-test.cc)

