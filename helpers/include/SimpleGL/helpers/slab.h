#ifndef SLAB_H
#define SLAB_H

#include <SimpleGL/resource.h>
#include <SimpleGL/texture.h>

namespace sgl {

template <GLenum kind>
struct Surface {
    sgl::Framebuffer fbo;
    sgl::Texture<kind> texture;

    Surface (sgl::Texture<kind>& texture) :
        texture(texture)
    {
        fbo.attachTexture(texture);
    }
};

template <GLenum kind>
struct Slab {
private:
    static_assert(traits::IsTexture<kind>::value
               || traits::IsRenderBuffer<kind>::value,
               "Supplied target must be a texture or renderbuffer");

    Surface<kind> _surfA;
    Surface<kind> _surfB;
    bool _useSurfA;

public:
    Slab (sgl::GLResource<kind>& a, sgl::GLResource<kind>& b) :
        _surfA(a),
        _surfB(b),
        _useSurfA(true)
    {}

    Slab (const sgl::Surface<kind>&& a, const sgl::Surface<kind>&& b) :
        _surfA(a),
        _surfB(b)
    {}

    Surface<kind>& ping () {
        if (_useSurfA) return _surfA;
        else return _surfB;
    }

    Surface<kind>& pong () {
        if (_useSurfA) return _surfB;
        else return _surfA;
    }

    void swap () {
        _useSurfA = !_useSurfA;
    }
};

using Surface2D = Surface<GL_TEXTURE_2D>;
using Slab2D    = Slab<GL_TEXTURE_2D>;

} // end namespace

#endif // SLAB_H
