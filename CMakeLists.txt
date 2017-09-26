cmake_minimum_required(VERSION 3.2)

project(SimpleGL3 CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 11)

set(SGL_USE_GLFW ON CACHE BOOL "Use GLFW as window api")
set(SGL_USE_GLEW ON CACHE BOOL "Use GLEW")
set(SGL_USE_GLM ON CACHE BOOL "Use GLM")
set(SGL_COMPILE_TESTS ON CACHE BOOL "Make test projects")
set(SGL_COMPILE_BENCHMARKS OFF CACHE BOOL "Make benchmarks")
set(SGL_DEBUG OFF CACHE BOOL "SGL Debug Mode")

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

if(${SGL_DEBUG})
    set(DEFINITIONS ${DEFINITIONS} -DSGL_DEBUG=1)
endif()




set(INCLUDE_FILES
    ${INCLUDE_DIR}/SimpleGL/SimpleGL.h
    ${INCLUDE_DIR}/SimpleGL/camera.h
    ${INCLUDE_DIR}/SimpleGL/context.h
    ${INCLUDE_DIR}/SimpleGL/contrib/stb_image.h
    ${INCLUDE_DIR}/SimpleGL/event.h
    ${INCLUDE_DIR}/SimpleGL/mesh.h
    ${INCLUDE_DIR}/SimpleGL/param.h
    ${INCLUDE_DIR}/SimpleGL/pbo.h
    ${INCLUDE_DIR}/SimpleGL/resource.h
    ${INCLUDE_DIR}/SimpleGL/sglconfig.h
    ${INCLUDE_DIR}/SimpleGL/shader.h
    ${INCLUDE_DIR}/SimpleGL/texture.h
    ${INCLUDE_DIR}/SimpleGL/traits.h
    ${INCLUDE_DIR}/SimpleGL/transform.h
    ${INCLUDE_DIR}/SimpleGL/utils.h
)

set(SOURCE_FILES
    ${SOURCE_DIR}/camera.cc
    ${SOURCE_DIR}/context.cc
    ${SOURCE_DIR}/event.cc
    ${SOURCE_DIR}/mesh.cc
    ${SOURCE_DIR}/shader.cc
    ${SOURCE_DIR}/traits.cc
    ${SOURCE_DIR}/transform.cc
    ${SOURCE_DIR}/utils.cc
)

set(OTHER_FILES
    README.md)


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
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${INCLUDE_FILES} ${OTHER_FILES})
target_link_libraries(${PROJECT_NAME} ${EXTERN_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})

if(${SGL_COMPILE_TESTS})
    add_subdirectory("./tests")
endif()

if(${SGL_COMPILE_BENCHMARKS})
    add_subdirectory("./benchmarks")
endif()
