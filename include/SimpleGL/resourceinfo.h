#ifndef BUFFERINFO_H
#define BUFFERINFO_H

#include "sglconfig.h"
#include "traits.h"
#include "utils.h"

namespace sgl {
namespace detail {

    struct GLBufferInfo {
        size_t length;
        GLenum usage;
    };

    struct GLRenderBufferInfo {
        GLsizei samples;
        GLenum iformat;
        size_t width, height;
        int redSize, greenSize, blueSize, alphaSize, depthSize, stencilSize;
    };

    struct GLTextureInfoBase {
        GLenum format = GL_RGB;
        GLenum iformat = GL_RGB;
        GLenum data_type = GL_UNSIGNED_BYTE;
        GLenum wrap_s = GL_CLAMP_TO_EDGE;
        GLenum wrap_t = GL_CLAMP_TO_EDGE;
        GLenum wrap_r = GL_CLAMP_TO_EDGE;
        GLenum min_filter = GL_NEAREST;
        GLenum mag_filter = GL_LINEAR;
        GLsizei levels = 0;
    };

    template <GLenum kind, class T = GLenum>
    struct GLTextureInfo : GLTextureInfoBase {};

    template <GLenum kind>
    struct GLTextureInfo<kind, traits::IfTex1D<kind>> : GLTextureInfoBase {
        int width;
        GLTextureInfo () : GLTextureInfoBase() {}
        GLTextureInfo (int w) : width(w), GLTextureInfoBase() {}
        GLTextureInfo (int w, const GLTextureInfoBase& info) : width(w), GLTextureInfoBase(info) {}
        size_t size () const { return width * sgl::traits::formatSize(iformat); }
    };

    template <GLenum kind>
    struct GLTextureInfo<kind, traits::IfTex2D<kind>> : GLTextureInfoBase {
        int width, height;
        GLTextureInfo () : GLTextureInfoBase() {}
        GLTextureInfo (int w, int h) : width(w), height(h), GLTextureInfoBase() {}
        GLTextureInfo (int w, int h, const GLTextureInfoBase& info) : width(w), height(h), GLTextureInfoBase(info) {}
        size_t size () const { return width * height * sgl::traits::formatSize(iformat); }
    };

    template <GLenum kind>
    struct GLTextureInfo<kind, traits::IfTex2DArray<kind>> : GLTextureInfoBase {
        int width, height, length;
        GLTextureInfo () : GLTextureInfoBase() {}
        GLTextureInfo (int w, int h, int length) : width(w), height(h), length(length), GLTextureInfoBase() {}
        GLTextureInfo (int w, int h, int length, const GLTextureInfoBase& info) : width(w), height(h), length(length), GLTextureInfoBase(info) {}
        size_t size () const { return width * height * sgl::traits::formatSize(iformat); }
    };

    template <GLenum kind>
    struct GLTextureInfo<kind, traits::IfTex3D<kind>> : GLTextureInfoBase {
        int width, height, depth;
        GLTextureInfo () : GLTextureInfoBase() {}
        GLTextureInfo (int w, int h, int d) : width(w), height(h), depth(d), GLTextureInfoBase() {}
        GLTextureInfo (int w, int h, int d, const GLTextureInfoBase& info) : width(w), height(h), depth(d), GLTextureInfoBase(info) {}
        size_t size () const { return width * height * depth * sgl::traits::formatSize(iformat); }
    };

    using GLTextureInfo1D      = GLTextureInfo<GL_TEXTURE_1D>;
    using GLTextureInfo2D      = GLTextureInfo<GL_TEXTURE_2D>;
    using GLTextureInfo2DArray = GLTextureInfo<GL_TEXTURE_CUBE_MAP>;
    using GLTextureInfo3D      = GLTextureInfo<GL_TEXTURE_3D>;

} // end namespace

template <GLenum kind, class _ = GLenum>
struct GLResourceInfo;

template <GLenum kind>
struct GLResourceInfo<kind, traits::IfTexture<kind>> { using type = detail::GLTextureInfo<kind>; };

template <GLenum kind>
struct GLResourceInfo<kind, traits::IfRenderBuffer<kind>> { using type = detail::GLRenderBufferInfo; };

template <GLenum kind>
struct GLResourceInfo<kind, traits::IfBuffer<kind>> { using type = detail::GLBufferInfo; };

// TODO: Not sure this class really helps with much. Consider removing it.
template <GLenum kind, class T = GLenum>
struct GLInfoInterface;

template <GLenum kind>
struct GLInfoInterface<kind, traits::IfBuffer<kind>> {
    GLint getParameter (GLenum param) {
        GLint res;
        glGetBufferParameteriv(kind, param, &res);
        return res;
    }

    GLint64 getParameter64 (GLenum param) {
        if (!config::sglOpenglVersion(3,2)) return getParameter(param);
        GLint64 res;
        glGetBufferParameteri64v(kind, param, &res);
        return res;
    }

    bool isValid (GLuint res) const { return glIsBuffer(res) != 0; }
    GLenum usage () const { return getParameter(GL_BUFFER_USAGE); }
    GLenum size () const { return getParameter(GL_BUFFER_SIZE); }
    GLint access () const { return getParameter(GL_BUFFER_ACCESS); }
    GLint accessFlags () const { return getParameter(GL_BUFFER_ACCESS_FLAGS); }
    bool isMapped () const { return getParameter(GL_BUFFER_MAPPED) != GL_FALSE; }
    GLint64 mappedSize () const { return getParameter64(GL_BUFFER_MAP_LENGTH); }
    GLint64 mappedOffset () const { return getParameter64(GL_BUFFER_MAP_OFFSET); }
    GLbitfield storageFlags () const { return getParameter(GL_BUFFER_STORAGE_FLAGS); }
    bool isImmutable () const { return getParameter(GL_BUFFER_IMMUTABLE_STORAGE); }
};

template <GLenum kind>
struct GLInfoInterface<kind, traits::IfTexture<kind>> {
    GLint getParameter (GLenum param) {
        GLint res;
        glGetTexParameteriv(kind, param, &res);
        return res;
    }

    GLfloat getParameterf (GLenum param) {
        if (!config::sglOpenglVersion(3,2)) return getParameter(param);
        GLfloat res;
        glGetTexParameterfv(kind, param, &res);
        return res;
    }

    GLenum depthStencilMode () const { return getParameter(GL_DEPTH_STENCIL_TEXTURE_MODE); }
    GLenum magFilter () const { return getParameter(GL_TEXTURE_MAG_FILTER); }
    GLenum minFilter () const { return getParameter(GL_TEXTURE_MIN_FILTER); }
    GLenum minLOD    () const { return getParameter(GL_TEXTURE_MIN_LOD); }
    GLenum maxLOD    () const { return getParameter(GL_TEXTURE_MAX_LOD); }
    GLenum baseLevel () const { return getParameter(GL_TEXTURE_BASE_LEVEL); }
    GLenum maxLevel  () const { return getParameter(GL_TEXTURE_MAX_LEVEL); }
    GLenum swizzleR  () const { return getParameter(GL_TEXTURE_SWIZZLE_R); }
    GLenum swizzleG  () const { return getParameter(GL_TEXTURE_SWIZZLE_G); }
    GLenum swizzleB  () const { return getParameter(GL_TEXTURE_SWIZZLE_B); }
    GLenum swizzleA  () const { return getParameter(GL_TEXTURE_SWIZZLE_A); }
    GLenum swizzleRGBA () const { return getParameter(GL_TEXTURE_SWIZZLE_RGBA); }
    GLenum wrapS () const { return getParameter(GL_TEXTURE_WRAP_S); }
    GLenum wrapT () const { return getParameter(GL_TEXTURE_WRAP_T); }
    GLenum wrapR () const { return getParameter(GL_TEXTURE_WRAP_R); }
    GLenum borderColor () const { return getParameter(GL_TEXTURE_BORDER_COLOR); }
    GLenum compareMode () const { return getParameter(GL_TEXTURE_COMPARE_MODE); }
    GLenum compareFunc () const { return getParameter(GL_TEXTURE_VIEW_MIN_LEVEL); }
    GLenum viewMinLevel () const { return getParameter(GL_TEXTURE_VIEW_MIN_LEVEL); }
    GLenum viewNumLevels () const { return getParameter(GL_TEXTURE_VIEW_NUM_LEVELS); }
    GLenum viewMinLayer () const { return getParameter(GL_TEXTURE_VIEW_MIN_LAYER); }
    GLenum viewNumLayers () const { return getParameter(GL_TEXTURE_VIEW_NUM_LAYERS); }
    GLenum immutableLevels () const { return getParameter(GL_TEXTURE_IMMUTABLE_LEVELS); }
    GLenum imageFormatCompatibilityType () const { return getParameter(GL_IMAGE_FORMAT_COMPATIBILITY_TYPE); }
    GLenum target () const { return getParameter(GL_TEXTURE_TARGET); }
};

template <GLenum kind>
struct GLInfoInterface<kind, traits::IfRenderBuffer<kind>> {
    GLint getParameter (GLenum param) {
        GLint res;
        glGetRenderbufferParameteriv(kind, param, &res);
        return res;
    }

    GLint width () const { return getParameter(GL_RENDERBUFFER_WIDTH); }
    GLint height () const { return getParameter(GL_RENDERBUFFER_HEIGHT); }
    GLint internalFormat () const { return getParameter(GL_RENDERBUFFER_INTERNAL_FORMAT); }
    GLint redSize () const { return getParameter(GL_RENDERBUFFER_RED_SIZE); }
    GLint greenSize () const { return getParameter(GL_RENDERBUFFER_GREEN_SIZE); }
    GLint blueSize () const { return getParameter(GL_RENDERBUFFER_BLUE_SIZE); }
    GLint alphaSize () const { return getParameter(GL_RENDERBUFFER_ALPHA_SIZE); }
    GLint depthSize () const { return getParameter(GL_RENDERBUFFER_DEPTH_SIZE); }
    GLint stencilSize () const { return getParameter(GL_RENDERBUFFER_STENCIL_SIZE); }
    GLint samples () const { return getParameter(GL_RENDERBUFFER_SAMPLES); }
};

} // end namespace

#endif // BUFFERINFO_H
