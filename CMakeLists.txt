cmake_minimum_required(VERSION 3.0)

project(SimpleGL3 CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_definitions(-std=c++11)

set(SGL_USE_GLFW ON CACHE BOOL "Use GLFW as window api")
set(SGL_USE_GLEW ON CACHE BOOL "Use GLEW")
set(SGL_USE_GLM ON CACHE BOOL "Use GLM")
set(SGL_COMPILE_TESTS OFF CACHE BOOL "Make test projects")

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

set(CMAKE_BUILD_TYPE Release)

set(DEFINITIONS "")

find_package(OpenGL REQUIRED)

if(${SGL_USE_GLFW})
    find_package(GLEW REQUIRED)
    set(DEFINITIONS ${DEFINITIONS} -DSGL_USE_GLFW=1)
endif()

if(${SGL_USE_GLEW})
    find_package(GLFW REQUIRED)
    set(DEFINITIONS ${DEFINITIONS} -DSGL_USE_GLEW=1)
endif()

if(${SGL_USE_GLM})
    find_package(GLM REQUIRED)
    set(DEFINITIONS ${DEFINITIONS} -DSGL_USE_GLM=1)
endif()



set(INCLUDE_FILES
    ${INCLUDE_DIR}/SimpleGL/SimpleGL.h
    ${INCLUDE_DIR}/SimpleGL/camera.h
    ${INCLUDE_DIR}/SimpleGL/context.h
    ${INCLUDE_DIR}/SimpleGL/format.h
    ${INCLUDE_DIR}/SimpleGL/mesh.h
    ${INCLUDE_DIR}/SimpleGL/param.h
    ${INCLUDE_DIR}/SimpleGL/resource.h
    ${INCLUDE_DIR}/SimpleGL/sglconfig.h
    ${INCLUDE_DIR}/SimpleGL/shader.h
    ${INCLUDE_DIR}/SimpleGL/texture.h
    ${INCLUDE_DIR}/SimpleGL/traits.h
    ${INCLUDE_DIR}/SimpleGL/transform.h
    ${INCLUDE_DIR}/SimpleGL/utils.h
    ${INCLUDE_DIR}/SimpleGL/window-provider.h
)

set(SOURCE_FILES
    ${SOURCE_DIR}/camera.cc
    ${SOURCE_DIR}/context.cc
    ${SOURCE_DIR}/event.cc
    ${SOURCE_DIR}/mesh.cc
    ${SOURCE_DIR}/shader.cc
    ${SOURCE_DIR}/transform.cc
    ${SOURCE_DIR}/utils.cc
)


#find_package(SOIL REQUIRED)

set(EXTERN_LIBRARIES ${GLFW_LIBRARIES}
                     ${GLEW_LIBRARIES}
                     ${SOIL_LIBRARIES}
                     ${OPENGL_LIBRARIES})

# OSX needs Cocoa
if (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    set(EXTERN_LIBRARIES "${EXTERN_LIBRARIES}" "${COCOA_LIBRARY}")
endif()


set(EXTERN_INCLUDES ${SOIL_INCLUDE_DIR}
                    ${GLM_INCLUDE_DIR}
                    ${GLEW_INCLUDE_DIR}
                    ${GLFW3_INCLUDE_DIR}
                    ${GLM_INCLUDE_DIR})


add_definitions(${DEFINITIONS})
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${INCLUDE_FILES})
target_link_libraries(${PROJECT_NAME} ${EXTERN_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})

macro(test_target name filename)
    add_executable(${name} ${filename})
    add_definitions(${DEFINITIONS})
    target_link_libraries(${name} ${PROJECT_NAME})
    target_include_directories(${name} PRIVATE ${PROJECT_NAME})
endmacro(test_target)

if(${SGL_COMPILE_TESTS})
    set(DEFINITIONS ${DEFINITIONS} 
        -DSGL_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests")

    test_target(context-test ${TEST_DIR}/context-test.cc)
    test_target(cube-test ${TEST_DIR}/cube-test.cc)
    test_target(framebuffer-test ${TEST_DIR}/framebuffer-test.cc)
    test_target(game-of-life ${TEST_DIR}/game-of-life.cc)
    test_target(key-test ${TEST_DIR}/key-test.cc)
    test_target(mouse-test ${TEST_DIR}/mouse-test.cc)
    test_target(param-test ${TEST_DIR}/param-test.cc)
    test_target(pbo-test ${TEST_DIR}/pbo-test.cc)
    test_target(plane-test ${TEST_DIR}/plane-test.cc)
    test_target(pointcloud-test ${TEST_DIR}/pointcloud-test.cc)
    test_target(resource-test ${TEST_DIR}/resource-test.cc)
    test_target(shader-test ${TEST_DIR}/shader-test.cc)
    test_target(size-test ${TEST_DIR}/size-test.cc)
    test_target(texture-test ${TEST_DIR}/texture-test.cc)
    test_target(traits-test ${TEST_DIR}/traits-test.cc)
endif()
