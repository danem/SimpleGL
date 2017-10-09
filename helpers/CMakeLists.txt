cmake_minimum_required(VERSION 3.2)
project(SimpleGLHelpers CXX)

find_package(GLFW REQUIRED)
find_package(GLM REQUIRED)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCE_FILES 
    ${SOURCE_DIR}/context.cc
    ${SOURCE_DIR}/camera.cc
    ${SOURCE_DIR}/event.cc
    ${SOURCE_DIR}/mesh.cc
    ${SOURCE_DIR}/transform.cc
)

set(HEADER_FILES
    ${INCLUDE_DIR}/SimpleGL/helpers/SimpleGLHelpers.h
    ${INCLUDE_DIR}/SimpleGL/helpers/context.h
    ${INCLUDE_DIR}/SimpleGL/helpers/camera.h
    ${INCLUDE_DIR}/SimpleGL/helpers/event.h
    ${INCLUDE_DIR}/SimpleGL/helpers/mesh.h
    ${INCLUDE_DIR}/SimpleGL/helpers/param.h
    ${INCLUDE_DIR}/SimpleGL/helpers/pbo.h
    ${INCLUDE_DIR}/SimpleGL/helpers/slab.h
    ${INCLUDE_DIR}/SimpleGL/helpers/transform.h
)

set(EXTERN_LIBRARIES
    ${GLFW_LIBRARIES}
    ${GLM_LIBRARIES}
    SimpleGL)

set(EXTERN_INCLUDES
    ${GLFW_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
    SimpleLG)

add_library(SimpleGLHelpers ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(SimpleGLHelpers PUBLIC ${EXTERN_LIBRARIES})
target_include_directories(SimpleGLHelpers PUBLIC ${EXTERN_INCLUDES} ${INCLUDE_DIR})


install(TARGETS SimpleGLHelpers  ARCHIVE DESTINATION lib)
install(DIRECTORY ${INCLUDE_DIR}/SimpleGL/helpers DESTINATION include/SimpleGL)

add_custom_target(install_SimpleGLHelpers
    COMMAND make install
)





