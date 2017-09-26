cmake_minimum_required(VERSION 3.2)
project(SGLTests)

set(CMAKE_CXX_STANDARD 11)

set(SHADER_FILES
    ./data/fs.glsl
    ./data/game-of-life_fs.glsl
    ./data/ident_vs.glsl
    ./data/instanced_vs.glsl
    ./data/pointcloud_fs.glsl
    ./data/pointcloud_vs.glsl
    ./data/texture_fs.glsl
    ./data/vs.glsl)

macro(test_target name filename)
    add_executable(${name} ${filename} ${SHADER_FILES})
    add_definitions(${DEFINITIONS})
    target_link_libraries(${name} PRIVATE SimpleGL3)
    target_include_directories(${name} PRIVATE SimpleGL3)
endmacro(test_target)

set(DEFINITIONS ${DEFINITIONS} 
    -DSGL_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

test_target(context-test     context-test.cc)
test_target(cube-test        cube-test.cc)
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
test_target(texture-test     texture-test.cc)
test_target(traits-test      traits-test.cc)
