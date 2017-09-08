#pragma once
#include "sglconfig.h"
#include "traits.h"

#include <memory>
#include <iostream>

namespace sgl {

namespace detail {
    // TODO: Hack to make vertex arrays conform with typical interface
    static inline void __glBindVertexArray (GLenum _, GLuint target) {
        __glewBindVertexArray(target);
    }

    // TODO: Hack to make shader programs conform with typical interface
    static inline void __glCreateProgram (int len, GLuint* dest) {
        for (int i = 0; i < len; i++){
            dest[i] = glCreateProgram();
        }
    }

    static inline void __glDeleteProgram (int len, const GLuint* dest) {
        for (int i = 0; i < len; i++){
            glDeleteProgram(dest[i]);
        }
    }

    static inline void __glUseProgram (GLenum _, GLuint target){
        glUseProgram(target);
    }

    using GLCreator = void (*) (int len, GLuint* dest);
    using GLDeleter = void (*) (int, const GLuint*);
    using GLBinder  = void (*) (GLenum, GLuint);

    // TODO: Figure out how to make these static...
    // This would make GLResource zero-overhead
    template <GLenum kind, class T = GLenum>
    struct GLInterface {};

    template <GLenum kind>
    struct GLInterface<kind, traits::IfBuffer<kind>> {
        const GLCreator create  = __glewGenBuffers;
        const GLDeleter destroy  = __glewDeleteBuffers;
        const GLBinder bind      = __glewBindBuffer;
    };

    template <GLenum kind>
    struct GLInterface<kind, traits::IfFramebuffer<kind>> {
        const GLCreator create  = __glewGenFramebuffers;
        const GLDeleter destroy = __glewDeleteFramebuffers;
        const GLBinder bind     = __glewBindFramebuffer;
    };

    template <GLenum kind>
    struct GLInterface<kind, traits::IfTexture<kind>> {
        const GLCreator create  = glGenTextures;
        const GLDeleter destroy = glDeleteTextures;
        const GLBinder bind     = glBindTexture;
    };

    template <GLenum kind>
    struct GLInterface<kind, traits::IfVertexArray<kind>> {
        const GLCreator create  = __glewGenVertexArrays;
        const GLDeleter destroy = __glewDeleteVertexArrays;
        const GLBinder bind     = detail::__glBindVertexArray;
    };

    template <GLenum kind>
    struct GLInterface<kind, traits::IfShaderProgram<kind>> {
        const GLCreator create = __glCreateProgram;
        const GLDeleter destroy = __glDeleteProgram;
        const GLBinder bind = __glUseProgram;
    };
} // end namespace


template <GLenum kind>
class GLResource {
protected:
    const detail::GLInterface<kind> _iface;
    GLuint _id;

public:
    static const GLenum type = kind;

    GLResource () {
        _iface.create(1, &_id);
    }

    GLResource (GLuint res) {
        _id = res;
    }

    operator GLuint() const { return _id; }

    void bind () {
        _iface.bind(kind, _id);
    }

    void unbind () {
        _iface.bind(kind, 0);
    }

    void release () {
        _iface.destroy(1,&_id);
    }
};


template <GLenum kind>
class BindGuard {
private:
    GLuint _res;
    const detail::GLInterface<kind> _iface;
    
public:
    BindGuard (GLuint res) :
        _res(res)
    {
        _iface.bind(kind,_res);
    }

    ~BindGuard () {
        _iface.bind(kind,0);
    }
};

template <GLenum kind>
BindGuard<kind> bind_guard (GLuint res) { return {res}; }

template <class T>
BindGuard<T::type> bind_guard (T& res) { return {(GLuint)res}; }

template <GLenum kind>
using GLSharedResource = std::shared_ptr<GLResource<kind>>;

template <GLenum kind>
GLSharedResource<kind> shared_resource () {
    return {new GLResource<kind>(), [](GLResource<kind>* r){ 
        r->release(); 
        delete r;
    }};
}

template <GLenum kind>
GLSharedResource<kind> shared_resource (GLResource<kind>& res) {
    return {&res, [](GLResource<kind>* r){ 
        r->release(); 
        delete r;
    }};
}

template <GLenum kind>
GLSharedResource<kind> shared_resource (GLuint res) {
    return {new GLResource<kind>(res), [](GLResource<kind>* r){ 
        r->release(); 
        delete r;
    }};
}



namespace detail {
    template <class D, GLenum kind, class T = GLenum>
    class BufferView;

    template <class D, GLenum kind>
    class BufferView<D, kind, traits::IfMappable<kind>>{
    private:
        const GLInterface<kind> _iface;
        GLuint _res;
        D* _data;

    public:
        BufferView (GLuint res, GLenum access) :
            _res(res)
        {
            _iface.bind(kind,_res);
            _data = static_cast<D*>(glMapBuffer(kind, access));
        }

        ~BufferView () {
            _iface.bind(kind,_res);
            glUnmapBuffer(kind);
            _iface.bind(kind,0);
        }

        D& operator[] (size_t idx) {
            return _data[idx];
        }

    };
}

template <class D, class R>
detail::BufferView<D, R::type> buffer_view (R& res, GLenum access = GL_READ_WRITE) {
    return {res,access};
}



using ArrayBuffer        = GLResource<GL_ARRAY_BUFFER>;
using ElementArrayBuffer = GLResource<GL_ELEMENT_ARRAY_BUFFER>;
using Framebuffer        = GLResource<GL_FRAMEBUFFER>;
using PackBuffer         = GLResource<GL_PIXEL_PACK_BUFFER>;
using QueryBuffer        = GLResource<GL_QUERY_BUFFER>;
using UniformBuffer      = GLResource<GL_UNIFORM_BUFFER>;
using UnpackBuffer       = GLResource<GL_PIXEL_UNPACK_BUFFER>;
using VertexArray        = GLResource<GL_VERTEX_ARRAY>;

using SArrayBuffer        = GLSharedResource<GL_ARRAY_BUFFER>;
using SElementArrayBuffer = GLSharedResource<GL_ELEMENT_ARRAY_BUFFER>;
using SFramebuffer        = GLSharedResource<GL_FRAMEBUFFER>;
using SPackBuffer         = GLSharedResource<GL_PIXEL_PACK_BUFFER>;
using SQueryBuffer        = GLSharedResource<GL_QUERY_BUFFER>;
using SUniformBuffer      = GLSharedResource<GL_UNIFORM_BUFFER>;
using SUnpackBuffer       = GLSharedResource<GL_PIXEL_UNPACK_BUFFER>;
using SVertexArray        = GLSharedResource<GL_VERTEX_ARRAY>;














} // end namespace
