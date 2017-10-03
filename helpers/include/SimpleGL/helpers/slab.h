#ifndef SLAB_H
#define SLAB_H

#include <SimpleGL/resource.h>
#include <SimpleGL/texture.h>

namespace sgl {

template <GLenum kind>
struct Surface {
    static_assert(traits::IsTexture<kind>::value
               || traits::IsRenderBuffer<kind>::value,
               "Supplied target must be a texture or renderbuffer");
    sgl::Framebuffer fbo;
    sgl::GLResource<kind> texture;

    Surface (sgl::GLResource<kind>& texture) :
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
