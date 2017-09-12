#pragma once
#include "sglconfig.h"
#include "traits.h"
#include "utils.h"

#include <memory>
#include <iostream>
#include <vector>

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
    bool _isBound;
    GLuint _id;

public:
    static const GLenum type = kind;

    GLResource () : 
        _isBound(false),
        _iface()
    {
        _iface.create(1, &_id);
    }

    GLResource (GLuint res) :
        _isBound(false),
        _iface()
    {
        _id = res;
    }

    operator GLuint() const { return _id; }

    void bind () {
        if (_isBound) return;
        _iface.bind(kind, _id);
        _isBound = true;
    }

    void unbind () {
        if (!_isBound) return;
        _iface.bind(kind, 0);
        _isBound = false;
    }

    bool isBound () {
        return _isBound;
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
        _res(res),
        _iface()
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

template <class Res>
GLSharedResource<Res::type> shared_resource () {
    return {new Res(), [](Res* r) {
        r->release();
        delete r;
    }};
}

template <class Res, class V>
GLSharedResource<Res::type> shared_resource (V value) {
    return {new Res(value), [](Res* r) {
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
} // end namespace

template <class D, class R>
detail::BufferView<D, R::type> buffer_view (R& res, GLenum access = GL_READ_WRITE) {
    return {res,access};
}

template <GLenum kind, class T>
class GLBuffer : public GLResource<kind> {
public:
    static_assert(traits::IsBuffer<kind>::value, "Invalid GL Type for buffer");
    using data_type = T;

    GLBuffer () : GLResource<kind>()
    {}

    GLBuffer (GLuint res) : GLResource<kind>(res)
    {}

    GLBuffer (std::vector<T>& data) : GLResource<kind>()
    {
        bufferData(*this, data);
    }

};

template <class T>
using ArrayBuffer = GLBuffer<GL_ARRAY_BUFFER, T>;

template <class T = GLuint>
using ElementArrayBuffer = GLBuffer<GL_ELEMENT_ARRAY_BUFFER, T>;


template <class R, class D>
void bufferData (R& res, std::vector<D>& data, GLenum usage = GL_DYNAMIC_DRAW) {
    static_assert(traits::IsBuffer<R::type>::value, "GLResource target must be buffer");
    bufferData(res,&data[0],data.size(),usage);
}

template <class R, class D, size_t Size>
void bufferData (R& res, std::array<D,Size>& data, GLenum usage = GL_DYNAMIC_DRAW) {
    static_assert(traits::IsBuffer<R::type>::value, "GLResource target must be buffer");
    bufferData(res,&data[0],Size,usage);
}

template <class R, class D>
void bufferData (R& res, D* data, size_t len, GLenum usage = GL_DYNAMIC_DRAW){
    static_assert(traits::IsBuffer<R::type>::value, "GLResource target must be buffer");
    res.bind();
    glBufferData(R::type, len * sizeof(D), data, usage);
}

template <GLenum kind, class D>
void bufferData (GLBuffer<kind, D>& buffer, std::vector<D>& data, GLenum usage = GL_DYNAMIC_DRAW){
    bufferData(buffer,&data[0],data.size(),usage);
}

template <GLenum kind, class D>
void bufferData (GLBuffer<kind, D>& buffer, D*  data, size_t len, GLenum usage = GL_DYNAMIC_DRAW){
    buffer.bind();
    glBufferData(buffer.type, len * sizeof(D), data, usage);
}


using VertexArray = GLResource<GL_VERTEX_ARRAY>;

class VertexAttribBuilder {
private:
    // Private to allow for automatic stride calculation.
    template <class T, class T2, class ...Ts>
    VertexAttribBuilder& addHelper (GLResource<GL_ARRAY_BUFFER>& res, int stride, bool normalized = false) {
        addHelper<T>(res,stride,normalized);
        addHelper<T2,Ts...>(res,stride,normalized);
        return *this;
    }

    template <class T>
    VertexAttribBuilder& addHelper (GLResource<GL_ARRAY_BUFFER>& res, int stride, bool normalized = false) {
        GLenum type = traits::GLType<T>::type;
        size_t elSize = sizeof(typename traits::CType<traits::GLType<T>::type>::type);
        int components = sizeof(T) / elSize;
        vao.bind();
        res.bind();
        glEnableVertexAttribArray(attribs);
        glVertexAttribPointer(attribs, components, type, normalized ? GL_TRUE : GL_FALSE, stride, (GLvoid*)offset);
        res.unbind();
        vao.unbind();
        offset += sizeof(T);
        attribs += 1;
        sglCatchGLError();
        return *this;
    }

public:
    VertexArray& vao;
    uint32_t attribs;
    uint32_t offset;

    VertexAttribBuilder (VertexArray& vao) :
        vao(vao),
        attribs(0),
        offset(0)
    {}

    template <class T, class T2, class ...Ts>
    VertexAttribBuilder& add (GLResource<GL_ARRAY_BUFFER>& res, bool normalized = false) {
        return addHelper<T,T2,Ts...>(res, traits::param_size<T,T2,Ts...>::size, normalized);
    }

};

class Framebuffer : public GLResource<GL_FRAMEBUFFER> {
public:
    Framebuffer () :
        GLResource<GL_FRAMEBUFFER>()
    {}

    template <GLenum kind>
    traits::IfTex1D<kind,void> attachTexture (GLResource<kind>& texture, GLenum attachment = GL_COLOR_ATTACHMENT0) {
        auto bg = bind_guard(*this);
        glFramebufferTexture1D(GL_FRAMEBUFFER, attachment, kind, (GLuint)texture, 0);
    }

    template <GLenum kind>
    traits::IfTex2D<kind,void> attachTexture (GLResource<kind>& texture, GLenum attachment = GL_COLOR_ATTACHMENT0) {
        auto bg = bind_guard(*this);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, kind, (GLuint)texture, 0);
    }
};




using PackBuffer         = GLResource<GL_PIXEL_PACK_BUFFER>;
using QueryBuffer        = GLResource<GL_QUERY_BUFFER>;
using UniformBuffer      = GLResource<GL_UNIFORM_BUFFER>;
using UnpackBuffer       = GLResource<GL_PIXEL_UNPACK_BUFFER>;

using SArrayBuffer        = GLSharedResource<GL_ARRAY_BUFFER>;
using SElementArrayBuffer = GLSharedResource<GL_ELEMENT_ARRAY_BUFFER>;
using SFramebuffer        = GLSharedResource<GL_FRAMEBUFFER>;
using SPackBuffer         = GLSharedResource<GL_PIXEL_PACK_BUFFER>;
using SQueryBuffer        = GLSharedResource<GL_QUERY_BUFFER>;
using SUniformBuffer      = GLSharedResource<GL_UNIFORM_BUFFER>;
using SUnpackBuffer       = GLSharedResource<GL_PIXEL_UNPACK_BUFFER>;
using SVertexArray        = GLSharedResource<GL_VERTEX_ARRAY>;

} // end namespace
