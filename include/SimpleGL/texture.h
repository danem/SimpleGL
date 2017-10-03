#pragma once

#include "sglconfig.h"
#include "resource.h"
#include "utils.h"
#include <initializer_list>


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
        size_t size () const { return width * sgl::traits::formatSize(iformat); }
    };

    template <GLenum kind>
    struct GLTextureInfo<kind, traits::IfTex2D<kind>> : GLTextureInfoBase {
        int width, height;
        GLTextureInfo () : GLTextureInfoBase() {}
        GLTextureInfo (int w, int h) : width(w), height(h), GLTextureInfoBase() {}
        GLTextureInfo (int w, int h, GLTextureInfoBase& info) : width(w), height(h), GLTextureInfoBase(info) {}
        size_t size () const { return width * height * sgl::traits::formatSize(iformat); }
    };

    template <GLenum kind>
    struct GLTextureInfo<kind, traits::IfTex2DArray<kind>> : GLTextureInfoBase {
        int width, height, length;
        GLTextureInfo () : GLTextureInfoBase() {}
        GLTextureInfo (int w, int h, int length) : width(w), height(h), length(length), GLTextureInfoBase() {}
        GLTextureInfo (int w, int h, int length, GLTextureInfoBase& info) : width(w), height(h), length(length), GLTextureInfoBase(info) {}
        size_t size () const { return width * height * sgl::traits::formatSize(iformat); }
    };

    template <GLenum kind>
    struct GLTextureInfo<kind, traits::IfTex3D<kind>> : GLTextureInfoBase {
        int width, height, depth;
        GLTextureInfo () : GLTextureInfoBase() {}
        GLTextureInfo (int w, int h, int d) : width(w), height(h), depth(d), GLTextureInfoBase() {}
        GLTextureInfo (int w, int h, int d, GLTextureInfoBase& info) : width(w), height(h), depth(d), GLTextureInfoBase(info) {}
        size_t size () const { return width * height * depth * sgl::traits::formatSize(iformat); }
    };

    using GLTextureInfo1D      = GLTextureInfo<GL_TEXTURE_1D>;
    using GLTextureInfo2D      = GLTextureInfo<GL_TEXTURE_2D>;
    using GLTextureInfo2DArray = GLTextureInfo<GL_TEXTURE_CUBE_MAP>;
    using GLTextureInfo3D      = GLTextureInfo<GL_TEXTURE_3D>;

    template <GLenum kind, class T = GLenum>
    struct GLTextureInterface;

    template <GLenum kind>
    struct GLTextureInterface<kind, traits::IfTex1D<kind>> {

        static void write (const void* data, const GLTextureInfo<kind>& info) {
            glTexImage1D(kind, 0, info.iformat, info.width, 0, info.format, info.data_type, data);
        }
        
        static void update (const void* data, const GLTextureInfo<kind>& info, int x = 0) {
            glTexSubImage1D(kind, 0, x, info.width, info.format, info.data_type, data);
        }
    };

    template <GLenum kind>
    struct GLTextureInterface<kind, traits::IfTex2D<kind>> {
        static void write (const void* data, const GLTextureInfo<kind>& info) {
            glTexImage2D(kind, 0, info.iformat, info.width, info.height, 0, info.format, info.data_type, data);
        }

        static void update (const void* data, const GLTextureInfo<kind>& info, int x = 0, int y = 0) {
            glTexSubImage2D(kind, 0, x, y, info.width, info.height, info.format, info.data_type, data);
        }
    };

    template <GLenum kind>
    struct GLTextureInterface<kind, traits::IfTex2DArray<kind>> {
        static void write (const void* data, const GLTextureInfo<kind>& info) {
            glTexImage2D(kind, 0, info.iformat, info.width, info.height, 0, info.format, info.data_type, data);
        }

        static void update (const void* data, const GLTextureInfo<kind>& info, int x = 0, int y = 0) {
            glTexSubImage2D(kind, 0, x, y, info.width, info.height, info.format, info.data_type, data);
        }
    };

    template <GLenum kind>
    struct GLTextureInterface<kind, traits::IfTex3D<kind>> {
        static void write (const void* data, const GLTextureInfo<kind>& info) {
            glTexImage3D(kind, 0, info.iformat, info.width, info.height, info.depth, 0, info.format, info.data_type, data);
        }

        static void update (const void* data, const GLTextureInfo<kind>& info, int x = 0, int y = 0, int z = 0) {
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


/**
* Texture provides a wrapper over an opengl texture object.
*/

 // TODO: Rework texture constructor. Also, reconsider storing all of the texture info along with
// the textures.
template <GLenum kind>
class Texture : public GLResource<kind> {
private:

    void applyParams () {
        glTexParameteri(kind, GL_TEXTURE_WRAP_S, attrs.wrap_s);
        glTexParameteri(kind, GL_TEXTURE_WRAP_T, attrs.wrap_t);
        glTexParameteri(kind, GL_TEXTURE_WRAP_R, attrs.wrap_r);
        glTexParameteri(kind, GL_TEXTURE_MIN_FILTER, attrs.min_filter);
        glTexParameteri(kind, GL_TEXTURE_MAG_FILTER, attrs.mag_filter);
    }

public:
    detail::GLTextureInfo<kind> attrs;

    Texture () {}

    // TODO: The write parameter is a hack. reconsider this.
    Texture (detail::GLTextureInfo<kind>& info, bool write = true) :
        GLResource<kind>(),
        attrs(info)
    {
        initialize(nullptr, attrs, write);
    }

    Texture (const void* data, detail::GLTextureInfo<kind>& info) :
        attrs(info),
        GLResource<kind>()
    {
        initialize(data, attrs, true);
    }

    void initialize (const void * data, detail::GLTextureInfo<kind>& info, bool write = true) {
        this->attrs = info;
        this->bind();
        if (write) detail::GLTextureInterface<kind>::write(data,attrs);
        applyParams();
        this->unbind();
        sglDbgCatchGLError();
    }
};

using Texture1D             = Texture<GL_TEXTURE_1D>;
using Texture2D             = Texture<GL_TEXTURE_2D>;
using Texture3D             = Texture<GL_TEXTURE_3D>;
using TextureCubeMap        = Texture<GL_TEXTURE_CUBE_MAP>;


/**
* TextureBuilder provides a convenient, typesafe way of
* creating opengl texture objects.
*
* example:
*
*   sgl::Texture1D tex1d = sgl::TextureBuilder<GL_TEXTURE_1D>()
*       .format(GL_R, GL_R8)
*       .build(1000);
*
*   sgl::Texture2D tex2d = sgl::TextureBuilder2D()
*       .build("image.png");
*
*   sgl::Texture2D tex2d2 = sgl::TextureBuilder2D()
*       .build(pixels, 500, 500);
*
*   sgl::Texture<GL_TEXTURE_3D> tex3d = sgl::TextureBuilder3D()
*       .wrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)
*       .build(500,500,500);
*
*   sgl::TextureCubeMap cubemap = sgl::TextureCubeMapBuilder()
*       .format(GL_RGBA,GL_RGBA)
*       .addImage(image1, w, h)
*       .addImage(image2, w, h)
*       .addImage(image3, w, h)
*       .build();
*
*/

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

    /*
    Texture<kind> build (const char * imagename) {
        int width, height, channels;
        unsigned char* data = detail::loadTexture2D(imagename, &width, &height, &channels);
        detail::GLTextureInfo<kind> info(width, height, this->_info);
        Texture<kind> tex(data,info);
        detail::freeTexture2D(data);
        return tex;
    }
    */

    Texture<kind> build (uint8_t * data, size_t width, size_t height) {
        detail::GLTextureInfo<kind> info(width, height, this->_info);
        Texture<kind> tex(data,info);
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

template <>
class TextureBuilder<GL_TEXTURE_CUBE_MAP, GLenum> :  public detail::TextureBuilderBase<TextureBuilder<GL_TEXTURE_CUBE_MAP>> {
private:
    TextureCubeMap _result;
    uint32_t _imageCount;
    int _width, _height;

public:
    TextureBuilder () :
        detail::TextureBuilderBase<TextureBuilder<GL_TEXTURE_CUBE_MAP>>(),
        _imageCount(0)
    {
        _result.bind();
    }

    using detail::TextureBuilderBase<TextureBuilder<GL_TEXTURE_CUBE_MAP>>::wrap;
    TextureBuilder<GL_TEXTURE_CUBE_MAP>& wrap (int s, int t, int r) = delete;

    TextureBuilder<GL_TEXTURE_CUBE_MAP>& addImage (const char * data, int w, int h, GLenum target = GL_DONT_CARE) {
        _width = w;
        _height = h;
        if (target == GL_DONT_CARE) target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + _imageCount;
        glTexImage2D(target, 0, _info.iformat, w, h, 0, _info.format, _info.data_type, data);
        _imageCount += 1;
        return *this;
    }

    TextureCubeMap build () {
        detail::GLTextureInfo2DArray info{_width, _height, static_cast<int>(_imageCount), _info};
        _result.initialize(nullptr, info, false);
        _result.unbind();
        return _result;
    }
};

using TextureBuilder1D      = TextureBuilder<GL_TEXTURE_1D>;
using TextureBuilder2D      = TextureBuilder<GL_TEXTURE_2D>;
using TextureBuilder3D      = TextureBuilder<GL_TEXTURE_3D>;
using TextureBuilderCubeMap = TextureBuilder<GL_TEXTURE_CUBE_MAP>;

template <GLenum kind>
void updateTexture (Texture<kind>& tex, const void * data) {
    auto bg = sgl::bind_guard(tex);
    detail::GLTextureInterface<kind>::update(data, tex.attrs);
    sglDbgCatchGLError();
}

template <GLenum kind>
void updateTexture (Texture<kind>& tex, const void * data, int x) {
    static_assert(traits::IsTex1D<kind>::value, "Texture must be 1D");
    auto bg = sgl::bind_guard(tex);
    detail::GLTextureInterface<kind>::update(data, tex.attrs, x);
    sglDbgCatchGLError();
}

template <GLenum kind>
void updateTexture (Texture<kind>& tex, const void * data, int x, int y) {
    static_assert(traits::IsTex2D<kind>::value, "Texture must be 2D");
    auto bg = sgl::bind_guard(tex);
    detail::GLTextureInterface<kind>::update(data, tex.attrs, x, y);
    sglDbgCatchGLError();
}

template <GLenum kind>
void updateTexture (Texture<kind>& tex, const void * data, int x, int y, int z) {
    static_assert(traits::IsTex3D<kind>::value, "Texture must be 3D");
    auto bg = sgl::bind_guard(tex);
    detail::GLTextureInterface<kind>::update(data, tex.attrs, x, y, z);
    sglDbgCatchGLError();
}

} // end namespace
