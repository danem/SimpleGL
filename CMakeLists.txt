cmake_minimum_required(VERSION 3.2)

project(SimpleGL3 CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 11)

set(SGL_USE_GLFW ON CACHE BOOL "Use GLFW as window api")
set(SGL_USE_GLEW OFF CACHE BOOL "Use GLEW for Opengl loading")
set(SGL_USE_LIBEPOXY ON CACHE BOOL "Use libepoxy for Opengl loading")
set(SGL_USE_GLM ON CACHE BOOL "Use GLM")
set(SGL_USE_GLES OFF CACHE BOOL "Enabled GLES")
set(SGL_COMPILE_TESTS ON CACHE BOOL "Make test projects")
set(SGL_COMPILE_BENCHMARKS OFF CACHE BOOL "Make benchmarks")
#set(SGL_DEBUG 0 CACHE STRING "SGL Debug Mode. Valid values [1-3]")
set(SGL_DEBUG_STATS OFF CACHE BOOL "Enable SGL OpenGL counters")

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

set(DEFINITIONS "")


find_package(OpenGL REQUIRED)

if(${SGL_USE_GLFW})
    find_package(GLFW REQUIRED)
    set(DEFINITIONS ${DEFINITIONS} -DSGL_USE_GLFW=1)
endif()


#if(${SGL_USE_GLEW})
#    find_package(GLEW REQUIRED)
#    set(DEFINITIONS ${DEFINITIONS} -DSGL_USE_GLEW=1)
#endif()

if(${SGL_USE_LIBEPOXY})
    find_package(EPOXY REQUIRED)
    set(DEFINITIONS ${DEFINITIONS} -DSGL_USE_EPOXY=1)
endif()

if(${SGL_USE_GLES})
    set(DEFINITIONS ${DEFINITIONS} -DSGL_USE_GLES=1)
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
    ${INCLUDE_DIR}/SimpleGL/utils.h
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
)

set(SOURCE_FILES
    ${SOURCE_DIR}/camera.cc
    ${SOURCE_DIR}/context.cc
    ${SOURCE_DIR}/event.cc
    ${SOURCE_DIR}/mesh.cc
    ${SOURCE_DIR}/shader.cc
    ${SOURCE_DIR}/texture.cc
    ${SOURCE_DIR}/traits.cc
    ${SOURCE_DIR}/transform.cc
    ${SOURCE_DIR}/utils.cc
)

set(OTHER_FILES
    README.md
    docs/lifetimes.md
    TODO.md)


#find_package(SOIL REQUIRED)

set(EXTERN_LIBRARIES ${GLFW_LIBRARIES}
                     ${GLEW_LIBRARIES}
                     ${OPENGL_LIBRARIES}
                     ${EPOXY_LIBRARIES})

# OSX needs Cocoa
if (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    set(EXTERN_LIBRARIES "${EXTERN_LIBRARIES}" "${COCOA_LIBRARY}")
endif()


set(EXTERN_INCLUDES ${GLM_INCLUDE_DIR}
                    ${GLEW_INCLUDE_DIR}
                    ${GLFW3_INCLUDE_DIR}
                    ${GLM_INCLUDE_DIR}
                    ${EPOXY_INCLUDE_DIR})


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

install(TARGETS SimpleGL3 ARCHIVE DESTINATION lib)
install(DIRECTORY ${INCLUDE_DIR}/SimpleGL DESTINATION include)

add_custom_target(install_SimpleLG3
    COMMAND make install
)
