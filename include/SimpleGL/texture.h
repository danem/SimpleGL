#pragma once

#include "sglconfig.h"
#include "resource.h"
#include "utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "contrib/stb_image.h"

namespace sgl {

namespace detail {

    struct GLTextureInfoBase {
        GLenum format = GL_RGB;
        GLenum iformat = GL_RGB;
        GLenum data_type = GL_UNSIGNED_BYTE;
        GLenum wrap_s = GL_CLAMP_TO_EDGE;
        GLenum wrap_t = GL_CLAMP_TO_EDGE;
        GLenum wrap_r = GL_CLAMP_TO_EDGE;
        GLenum min_filter = GL_NEAREST;
        GLenum mag_filter = GL_LINEAR;
    };

    template <GLenum kind, class T = GLenum>
    struct GLTextureInfo : GLTextureInfoBase {};

    template <GLenum kind>
    struct GLTextureInfo<kind, traits::IfTex1D<kind>> : GLTextureInfoBase {
        int width;
        GLTextureInfo () : GLTextureInfoBase() {}
        GLTextureInfo (int w) : width(w), GLTextureInfoBase() {}
        GLTextureInfo (int w, GLTextureInfoBase& info) : width(w), GLTextureInfoBase(info) {}
    };

    template <GLenum kind>
    struct GLTextureInfo<kind, traits::IfTex2D<kind>> : GLTextureInfoBase {
        int width, height;
        GLTextureInfo () : GLTextureInfoBase() {}
        GLTextureInfo (int w, int h) : width(w), height(h), GLTextureInfoBase() {}
        GLTextureInfo (int w, int h, GLTextureInfoBase& info) : width(w), height(h), GLTextureInfoBase(info) {}
    };

    template <GLenum kind>
    struct GLTextureInfo<kind, traits::IfTex3D<kind>> : GLTextureInfoBase {
        int width, height, depth;
        GLTextureInfo () : GLTextureInfoBase() {}
        GLTextureInfo (int w, int h, int d) : width(w), height(h), depth(d), GLTextureInfoBase() {}
        GLTextureInfo (int w, int h, int d, GLTextureInfoBase& info) : width(w), height(h), depth(d), GLTextureInfoBase(info) {}
    };

    template <GLenum kind, class T = GLenum>
    struct GLTextureInterface;

    template <GLenum kind>
    struct GLTextureInterface<kind, traits::IfTex1D<kind>> {

        static void write (const void* data, const GLTextureInfo<kind>& info) {
            glTexImage1D(kind, 0, info.iformat, info.width, 0, info.format, info.data_type, data);
        }
        
        static void update (const void* data, const GLTextureInfo<kind>& info, int x = 0) {
            glTexSubImage1D(kind, info.iformat, x, info.width, 0, info.format, info.data_type, data);
        }
    };

    template <GLenum kind>
    struct GLTextureInterface<kind, traits::IfTex2D<kind>> {
        void write (const void* data, const GLTextureInfo<kind>& info) {
            glTexImage2D(kind, 0, info.iformat, info.width, info.height, 0, info.format, info.data_type, data);
        }

        void update (const void* data, const GLTextureInfo<kind>& info, int x = 0, int y = 0) {
            glTexSubImage2D(kind, 0, x, y, info.width, info.height, info.format, info.data_type, data);
        }
    };

    template <GLenum kind>
    struct GLTextureInterface<kind, traits::IfTex3D<kind>> {
        void write (const void* data, const GLTextureInfo<kind>& info) {
            glTexImage3D(kind, 0, info.iformat, info.width, info.height, info.depth, 0, info.format, info.data_type, data);
        }

        void update (const void* data, const GLTextureInfo<kind>& info, int x = 0, int y = 0, int z = 0) {
            glTexSubImage3D(kind, 0, x, y, z, info.width, info.height, info.depth, info.format, info.data_type, data);
        }
    };

    template <class T>
    class TextureBuilderBase {
    private:

    public:
        GLTextureInfoBase _info;

        T& wrap (GLenum s, GLenum t, GLenum r) {
            _info.wrap_s = s;
            _info.wrap_t = t;
            _info.wrap_r = r;
            return static_cast<T&>(*this);
        }

        T& wrap (GLenum s, GLenum t) {
            _info.wrap_s = s;
            _info.wrap_t = t;
            return static_cast<T&>(*this);
        }

        T& wrap (GLenum s) {
            _info.wrap_s = s;
            return static_cast<T&>(*this);
        }

        T& filter (int min, int mag) {
            _info.min_filter = min;
            _info.mag_filter = mag;
            return static_cast<T&>(*this);
        }
        
        T& internalFormat (int fmt) {
            _info.iformat = fmt;
            return static_cast<T&>(*this);
        }

        T& format (int fmt) {
            _info.format = fmt;
            return static_cast<T&>(*this);
        }

        T& format (int fmt, int ifmt) {
            _info.format = fmt;
            _info.iformat = ifmt;
            return static_cast<T&>(*this);
        }

        T& dataType (GLenum type) {
            _info.data_type = type;
            return static_cast<T&>(*this);
        }
    };
} // end namespace

template <GLenum kind>
class Texture : public GLResource<kind> {
private:
    detail::GLTextureInterface<kind> _texIface;

    void applyParams () {
        glTexParameteri(kind, GL_TEXTURE_WRAP_S, attrs.wrap_s);
        glTexParameteri(kind, GL_TEXTURE_WRAP_T, attrs.wrap_t);
        glTexParameteri(kind, GL_TEXTURE_WRAP_R, attrs.wrap_r);
        glTexParameteri(kind, GL_TEXTURE_MIN_FILTER, attrs.min_filter);
        glTexParameteri(kind, GL_TEXTURE_MAG_FILTER, attrs.mag_filter);
    }

public:
    const detail::GLTextureInfo<kind> attrs;

    Texture (detail::GLTextureInfo<kind>& info) :
        attrs(info),
        GLResource<kind>()
    {
        this->bind();
        _texIface.write(nullptr, attrs);
        applyParams();
        this->unbind();
        sglCatchGLError();
    }

    Texture (const void* data, detail::GLTextureInfo<kind>& info) :
        attrs(info),
        GLResource<kind>()
    {
        this->bind();
        _texIface.write(data, attrs);
        applyParams();
        this->unbind();
        sglCatchGLError();
    }
};


template <GLenum kind, class T = GLenum>
class TextureBuilder;

template <GLenum kind>
class TextureBuilder<kind, traits::IfTex1D<kind>> : public detail::TextureBuilderBase<TextureBuilder<kind>> {
public:
    TextureBuilder () : 
        detail::TextureBuilderBase<TextureBuilder<kind>>()
    {}

    using detail::TextureBuilderBase<TextureBuilder<kind>>::wrap;
    TextureBuilder<kind>& wrap (int s, int t, int r) = delete;
    TextureBuilder<kind>& wrap (int s, int t) = delete;

    Texture<kind> build (size_t width) {
        detail::GLTextureInfo<kind> info(width, this->_info);
        return {info};
    }
};

template <GLenum kind>
class TextureBuilder<kind, traits::IfTex2D<kind>> : public detail::TextureBuilderBase<TextureBuilder<kind>> {
public:
    TextureBuilder () : 
        detail::TextureBuilderBase<TextureBuilder<kind>>()
    {}

    using detail::TextureBuilderBase<TextureBuilder<kind>>::wrap;
    TextureBuilder<kind>& wrap (int s, int t, int r) = delete;

    Texture<kind> build (size_t width, size_t height) {
        detail::GLTextureInfo<kind> info(width, height, this->_info);
        return {info};
    }

    Texture<kind> build (const char * imagename) {
        int width, height, channels;
        unsigned char* data = stbi_load(imagename, &width, &height, &channels, 0);
        detail::GLTextureInfo<kind> info(width, height, this->_info);
        Texture<kind> tex(data,info);
        stbi_image_free(data);
        return tex;
    }
};

template <GLenum kind>
class TextureBuilder<kind, traits::IfTex3D<kind>> : public detail::TextureBuilderBase<TextureBuilder<kind>> {
public:
    TextureBuilder () : 
        detail::TextureBuilderBase<TextureBuilder<kind>>()
    {}

    using detail::TextureBuilderBase<TextureBuilder<kind>>::wrap;

    Texture<kind> build (size_t width, size_t height, size_t depth) {
        detail::GLTextureInfo<kind> info(width, height, depth, this->_info);
        return {info};
    }
};

using Texture1D        = Texture<GL_TEXTURE_1D>;
using Texture2D        = Texture<GL_TEXTURE_2D>;
using Texture3D        = Texture<GL_TEXTURE_3D>;
using Texture1DBuilder = TextureBuilder<GL_TEXTURE_1D>;
using Texture2DBuilder = TextureBuilder<GL_TEXTURE_2D>;
using Texture3DBuilder = TextureBuilder<GL_TEXTURE_3D>;


} // end namespace
