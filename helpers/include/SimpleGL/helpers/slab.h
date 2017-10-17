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
        fbo.unbind();
    }

    void release () {
        fbo.release();
        texture.release();
    }
};

template <GLenum kind>
struct Slab {
private:

    Surface<kind> _surfA;
    Surface<kind> _surfB;
    bool _useSurfA;

public:
    Slab (sgl::Texture<kind>& a, sgl::Texture<kind>& b) :
        _surfA(a),
        _surfB(b),
        _useSurfA(true)
    {}

    Slab (const sgl::Surface<kind>& a, const sgl::Surface<kind>& b) :
        _surfA(a),
        _surfB(b),
        _useSurfA(true)
    {}

    Slab (const sgl::Surface<kind>&& a, const sgl::Surface<kind>&& b) :
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

    void release () {
        _surfA.release();
        _surfB.release();
    }
};

using Surface2D = Surface<GL_TEXTURE_2D>;
using Slab2D    = Slab<GL_TEXTURE_2D>;


} // end namespace

#endif // SLAB_H
