#include <SimpleGL/traits.h>


size_t sgl::traits::formatSize (GLenum fmt) {
    switch(fmt) {
    case GL_R: return 1;
    case GL_R8: return 1;
    case GL_R8_SNORM: return 1;
    case GL_R16F: return 2;
    case GL_R32F: return 4;
    case GL_R8UI: return 1;
    case GL_R8I: return 1;
    case GL_R16UI: return 2;
    case GL_R16I: return 2;
    case GL_R32UI: return 4;
    case GL_R32I: return 4;

    case GL_RG: return 2;
    case GL_RG8: return 2;
    case GL_RG8_SNORM: return 2;
    case GL_RG16F: return 4;
    case GL_RG32F: return 8;
    case GL_RG8UI: return 2;
    case GL_RG8I: return 2;
    case GL_RG16UI: return 4;
    case GL_RG16I: return 4;
    case GL_RG32UI: return 8;
    case GL_RG32I: return 8;

    case GL_RGB: return 3;
    case GL_RGB8: return 3;
    case GL_SRGB8: return 2;
    case GL_RGB565: return 2;
    case GL_RGB8_SNORM: return 2;
    case GL_R11F_G11F_B10F: return 4;
    case GL_RGB9_E5: return 4;
    case GL_RGB16F: return 6;
    case GL_RGB32F: return 12;
    case GL_RGB8UI: return 3;
    case GL_RGB8I: return 3;
    case GL_RGB16UI: return 6;
    case GL_RGB16I: return 6;
    case GL_RGB32UI: return 12;
    case GL_RGB32I: return 12;

    case GL_RGBA: return 4;
    case GL_RGBA8: return 4;
    case GL_SRGB8_ALPHA8: return 4;
    case GL_RGBA8_SNORM: return 4;
    case GL_RGB5_A1: return 2;
    case GL_RGBA4: return 2;
    case GL_RGB10_A2: return 4;
    case GL_RGBA16F: return 8;
    case GL_RGBA32F: return 16;
    case GL_RGBA8UI: return 4;
    case GL_RGBA8I: return 4;
    case GL_RGB10_A2UI: return 4;
    case GL_RGBA16UI: return 8;
    case GL_RGBA16I: return 8;
    case GL_RGBA32UI: return 16;
    case GL_RGBA32I: return 16;
    case GL_LUMINANCE: return 1;
    case GL_LUMINANCE_ALPHA: return 1;
    default: return 1;
    }
}
