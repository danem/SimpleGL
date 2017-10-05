#pragma once

#include <initializer_list>
#include <vector>
#include <array>

#include "sglconfig.h"
#include "utils.h"
#include "resourceinfo.h"
#include "resource.h"


namespace sgl {

namespace detail {

    template <GLenum kind, class T = GLenum>
    struct GLTextureInterface;

    template <GLenum kind>
    struct GLTextureInterface<kind, traits::IfTex1D<kind>> {
        static inline void allocate (const GLTextureInfo<kind>& info) {
            if (SGL_TEXSTORAGE_SUPPORTED) {
                glTexStorage1D(kind, info.levels, info.iformat, info.width);
                sglDbgLogVerbose("glTexStorage1D(%d, %lu, %d, %d)\n", kind, info.levels, info.iformat, info.width);
            }
            else allocateMut(info);
        }

        static inline void allocateMut (const GLTextureInfo<kind>& info) {
            write(NULL,info);
        }

        static inline void write (const void* data, const GLTextureInfo<kind>& info) {
            glTexImage1D(kind, 0, info.iformat, info.width, 0, info.format, info.data_type, data);
            sglDbgLogVerbose("glTexImage1D(%d, 0, %d, %d, 0, %d, %d, NULL)\n", kind, info.iformat, info.width, info.format, info.data_type);
        }
        
        static inline void update (const void* data, const GLTextureInfo<kind>& info, int x = 0) {
            glTexSubImage1D(kind, 0, x, info.width, info.format, info.data_type, data);
            sglDbgLogVerbose("glTexSubImage1D(%d, 0, %d, %d, %d, %d, %p)\n", kind, x, info.width, info.format, info.data_type, data);
        }
    };

    template <GLenum kind>
    struct GLTextureInterface<kind, traits::IfTex2D<kind>> {
        static inline void allocate (const GLTextureInfo<kind>& info) {
            if (SGL_TEXSTORAGE_SUPPORTED) {
                glTexStorage2D(kind, info.levels, info.iformat, info.width, info.height);
            } else allocateMut(info);
        }

        static inline void allocateMut (const GLTextureInfo<kind>& info) {
            write(NULL,info);
        }

        static inline void write (const void* data, const GLTextureInfo<kind>& info) {
            glTexImage2D(kind, 0, info.iformat, info.width, info.height, 0, info.format, info.data_type, data);
        }

        static inline void update (const void* data, const GLTextureInfo<kind>& info, int x = 0, int y = 0) {
            glTexSubImage2D(kind, 0, x, y, info.width, info.height, info.format, info.data_type, data);
        }
    };

    template <GLenum kind>
    struct GLTextureInterface<kind, traits::IfTex2DArray<kind>> {
        static inline void allocate (const GLTextureInfo<kind>& info) {
            if (SGL_TEXSTORAGE_SUPPORTED) {
                glTexStorage3D(kind, info.levels, info.iformat, info.width, info.height, info.length);
            } else allocateMut(info);
        }

        static inline void allocateMut (const GLTextureInfo<kind>& info) {
            write(NULL,info);
        }

        static inline void write (const void* data, const GLTextureInfo<kind>& info) {
            glTexImage3D(kind, 0, info.iformat, info.width, info.height, info.length, 0, info.format, info.data_type, data);
        }

        static inline void update (const void* data, const GLTextureInfo<kind>& info, int x = 0, int y = 0) {
            glTexSubImage3D(kind, 0, x, y, info.width, info.height, info.length, info.format, info.data_type, data);
        }
    };

    template <GLenum kind>
    struct GLTextureInterface<kind, traits::IfTex3D<kind>> {


        static inline void write (const void* data, const GLTextureInfo<kind>& info) {
            glTexImage3D(kind, 0, info.iformat, info.width, info.height, info.depth, 0, info.format, info.data_type, data);
        }

        static inline void update (const void* data, const GLTextureInfo<kind>& info, int x = 0, int y = 0, int z = 0) {
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

using TextureLoader = const unsigned char * (const char * path, int * width, int * height, int * channels, int);
using TextureFreer  = void (const unsigned char *);

struct TextureAccessor {
    TextureLoader loader;
    TextureFreer freer;
};


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

using Texture1D      = Texture<GL_TEXTURE_1D>;
using Texture2D      = Texture<GL_TEXTURE_2D>;
using Texture3D      = Texture<GL_TEXTURE_3D>;
using TextureCubeMap = Texture<GL_TEXTURE_CUBE_MAP>;


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

    Texture<kind> build (const char * imagename, TextureLoader loader, TextureFreer freer) {
        int width, height, channels;
        const unsigned char* data = loader(imagename, &width, &height, &channels, 0);
        detail::GLTextureInfo<kind> info(width, height, this->_info);
        Texture<kind> tex(data,info);
        freer(data);
        return tex;
    }

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

    TextureBuilder<GL_TEXTURE_CUBE_MAP>& addImage (const uint8_t * data, int w, int h, GLenum target = GL_DONT_CARE) {
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

    TextureCubeMap build (TextureAccessor& loader, std::vector<const char *> paths) {
        return build(loader, &paths[0], paths.size());
    }

    TextureCubeMap build (TextureAccessor& loader, const char** paths, size_t len){
        for (size_t i = 0; i < len; i++){
            int w, h, c;
            const unsigned char * data = loader.loader(paths[i], &w, &h, &c, 0);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, _info.iformat, w, h, 0, _info.format, _info.data_type, data);
            loader.freer(data);
        }
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
