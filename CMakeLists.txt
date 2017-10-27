cmake_minimum_required(VERSION 3.2)


set(SGL_USE_ANDROID OFF CACHE BOOL "Build for android")
set(SGL_USE_GLES OFF CACHE BOOL "Enabled GLES")
set(SGL_COMPILE_HELPERS ON CACHE BOOL "Make SimpleGL Helper Library. Requires GLM and GLFW3")
set(SGL_COMPILE_TESTS ON CACHE BOOL "Make test projects")
set(SGL_COMPILE_BENCHMARKS OFF CACHE BOOL "Make benchmarks")
#set(SGL_DEBUG 0 CACHE STRING "SGL Debug Mode. Valid values [1-3]")


project(SimpleGL CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 11)


set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

set(DEFINITIONS "")

find_package(OpenGL REQUIRED)
find_package(EPOXY REQUIRED)

if(${SGL_USE_GLES})
    set(DEFINITIONS ${DEFINITIONS} -DSGL_USE_GLES=1)
endif()

if(${SGL_DEBUG})
    set(DEFINITIONS ${DEFINITIONS} -DSGL_DEBUG=1)
endif()

if (${SGL_USE_ANDROID})
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
endif()
    set(ANDROID_STL stlport_static)


set(INCLUDE_FILES
    ${INCLUDE_DIR}/SimpleGL/SimpleGL.h
    ${INCLUDE_DIR}/SimpleGL/utils.h
    ${INCLUDE_DIR}/SimpleGL/resource.h
    ${INCLUDE_DIR}/SimpleGL/resourceinfo.h
    ${INCLUDE_DIR}/SimpleGL/sglconfig.h
    ${INCLUDE_DIR}/SimpleGL/shader.h
    ${INCLUDE_DIR}/SimpleGL/texture.h
    ${INCLUDE_DIR}/SimpleGL/traits.h
)

set(SOURCE_FILES
    ${SOURCE_DIR}/sglconfig.cc
    ${SOURCE_DIR}/shader.cc
    ${SOURCE_DIR}/traits.cc
    ${SOURCE_DIR}/utils.cc
)

set(OTHER_FILES
    README.md
    docs/lifetimes.md
    TODO.md
)


set(EXTERN_LIBRARIES ${OPENGL_LIBRARIES}
                     ${EPOXY_LIBRARIES})

# OSX needs Cocoa
if (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    set(EXTERN_LIBRARIES "${EXTERN_LIBRARIES}" "${COCOA_LIBRARY}")
endif()


set(EXTERN_INCLUDES ${EPOXY_INCLUDE_DIR})


add_definitions(${DEFINITIONS})
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${INCLUDE_FILES} ${OTHER_FILES})
target_link_libraries(${PROJECT_NAME} ${EXTERN_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR} ${EXTERN_INCLUDES})

if (${SGL_COMPILE_HELPERS})
    if(${SGL_USE_ANDROID})
        message(FATAL_ERROR "Helper library is currently unavailable for android, as it relies on GLFW.")
    endif()
    add_subdirectory("./helpers")
endif()

if(${SGL_COMPILE_TESTS})
    if (NOT ${SGL_COMPILE_HELPERS})
        message(FATAL_ERROR "You must enable SGL_COMPILE_HELPERS to make the examples")
    endif()
    add_subdirectory("./tests")
endif()

if(${SGL_COMPILE_BENCHMARKS})
    add_subdirectory("./benchmarks")
endif()

install(TARGETS SimpleGL ARCHIVE DESTINATION lib)
install(DIRECTORY ${INCLUDE_DIR}/SimpleGL DESTINATION include)

if (WIN32)
    add_custom_target(install_SimpleGL
        COMMAND make install
    )
else()
    add_custom_target(install_SimpleGL
        COMMAND make install
    )
endif()
