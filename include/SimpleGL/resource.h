#pragma once
#include "sglconfig.h"
#include "traits.h"
#include "utils.h"

#include <memory>
#include <map>
#include <iostream>
#include <vector>
#include <type_traits>

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
        static void create (int len, GLuint* dest) { __glewGenBuffers(len,dest); }
        static void destroy (int len, GLuint* dest) { __glewDeleteBuffers(len,dest); }
        static void bind (GLenum k, GLuint id) { __glewBindBuffer(k,id); }
    };

    template <GLenum kind>
    struct GLInterface<kind, traits::IfFramebuffer<kind>> {
        static void create (int len, GLuint* dest) { __glewGenFramebuffers(len,dest); }
        static void destroy (int len, GLuint* dest) { __glewDeleteFramebuffers(len,dest); }
        static void bind (GLenum k, GLuint id) { __glewBindFramebuffer(k,id); }
    };

    template <GLenum kind>
    struct GLInterface<kind, traits::IfTexture<kind>> {
        static void create (int len, GLuint* dest) { glGenTextures(len,dest); }
        static void destroy (int len, GLuint* dest) { glDeleteTextures(len,dest); }
        static void bind (GLenum k, GLuint id) { glBindTexture(k,id); }
    };

    template <GLenum kind>
    struct GLInterface<kind, traits::IfVertexArray<kind>> {
        static void create (int len, GLuint* dest) { __glewGenVertexArrays(len,dest); }
        static void destroy (int len, GLuint* dest) { __glewDeleteVertexArrays(len,dest); }
        static void bind (GLenum k, GLuint id) { detail::__glBindVertexArray(k,id); }
    };

    template <GLenum kind>
    struct GLInterface<kind, traits::IfShaderStage<kind>> {
        static void create (int len, GLuint* dest) { *dest = glCreateShader(kind); }
        static void destroy (int len, GLuint* dest) { glDeleteShader(*dest); }
        static void bind (GLenum k, GLuint id) {}
    };

    template <GLenum kind>
    struct GLInterface<kind, traits::IfShaderProgram<kind>> {
        static void create (int len, GLuint* dest) { __glCreateProgram(len,dest); }
        static void destroy (int len, GLuint* dest) { __glDeleteProgram(len,dest); }
        static void bind (GLenum k, GLuint id) { __glUseProgram(k,id); }
    };

    /*
    template <GLenum kind, class T = GLenum>
    struct GLBufferInterface;

    template <GLenum kind>
    struct GLBufferInterface<kind, traits::IfBuffer<kind>> {
        static inline void initializeMutable (size_t len, char * data, GLenum mode) { glBufferData(kind, len, data, mode); }
        static inline void initializeImmutable (size_t len, char * data, GLbitfield mode) { glBufferStorage(kind, len, data, mode); }
        static inline void update (GLintptr offset, GLsizeiptr size, const GLvoid * data) { glBufferSubData(kind, offset, size, data); }
        static inline void* map (GLenum mode) { return glMapBuffer(kind,mode); }
        static inline void unmap () { glUnmapBuffer(kind); }
        static inline void* mapRange (GLintptr offset, GLsizeiptr size, GLbitfield mode) { return glMapBufferRange(kind, offset, size, mode); }
    };

    template <GLenum kind>
    struct GLBufferInterface<kind,traits::IsTexture<kind>> {
        static inline void initializeMutable (size_t len, char * data, GLenum mode) { glBufferData(kind, len, data, mode); }
        static inline void initializeImmutable (size_t len, char * data, GLbitfield mode) { glBufferStorage(kind, len, data, mode); }
        static inline void update (GLintptr offset, GLsizeiptr size, const GLvoid * data) { glBufferSubData(kind, offset, size, data); }
        static inline void* map (GLenum mode) { return glMapBuffer(kind,mode); }
        static inline void unmap () { glUnmapBuffer(kind); }
        static inline void* mapRange (GLintptr offset, GLsizeiptr size, GLbitfield mode) { return glMapBufferRange(kind, offset, size, mode); }
    };
    */
} // end namespace


// Core resource management class. Only uses 8 bytes compared to GLuint's 4.
template <GLenum kind>
class GLResource {
protected:
    bool _isBound;
    GLuint _id;

public:
    static const GLenum type = kind;

    GLResource () : 
        _isBound(false)
    {
        detail::GLInterface<kind>::create(1, &_id);
    }

    GLResource (GLuint res) :
        _isBound(false),
        _id(res)
    {}

    operator GLuint() const { return _id; }

    void bind () {
        //if (_isBound) return;
        detail::GLInterface<kind>::bind(kind, _id);
        _isBound = true;
    }

    void unbind () {
        //if (!_isBound) return;
        detail::GLInterface<kind>::bind(kind, 0);
        _isBound = false;
    }

    bool isBound () {
        return _isBound;
    }

    void release () {
        detail::GLInterface<kind>::destroy(1, &_id);
    }
};

template <GLenum kind>
class GLResourceArray {
private:
    size_t _size;
    GLuint * _ids;

public:

    GLResourceArray (size_t size) :
        _size(size)
    {
        _ids = new GLuint[size]();
        detail::GLInterface<kind>::create(_size,_ids);
    }

    void release () {
        detail::GLInterface<kind>::destroy(_size,_ids);
        delete _ids;
    }

    GLResource<kind> operator[] (size_t idx) {
        return {_ids[idx]};
    }

    void bind (size_t idx) {
        detail::GLInterface<kind>::bind(kind, _ids[idx]);
    }

    void unbind (size_t idx) {
        detail::GLInterface<kind>::bind(kind, 0);
    }
};

template <class T>
class GLResourceM {
private:
    static_assert(std::is_base_of<GLResource<T::type>,T>::value, "Must be GLResource");
    T _res;

public:

    GLResourceM () {}

    template <class ...Ts>
    GLResourceM (Ts... args) :
        _res(args...)
    {}

    ~GLResourceM () {
        _res.release();
    }

    T* operator-> () { return &_res; }

    operator T&() const { return _res; }
    operator GLuint() const { return static_cast<GLuint>(_res); }


    T& get () const { return _res; }
};

template <GLenum kind>
class BindGuard {
private:
    GLuint _res;
    // TODO: A bit of a hack to cut down on unecessary binds
    bool _alreadyBound;

public:
    BindGuard (GLuint res, bool alreadyBound = false) :
        _res(res),
        _alreadyBound(alreadyBound)
    {
        if (!_alreadyBound) detail::GLInterface<kind>::bind(kind, _res);
    }

    ~BindGuard () {
        if (!_alreadyBound) detail::GLInterface<kind>::bind(kind, 0);
    }
};

template <GLenum kind>
class ResourceGuard {
private:
    GLResource<kind> _res;

public:
    ResourceGuard (GLResource<kind> res) :
        _res(res)
    {}

    ~ResourceGuard () {
        _res.release();
    }

    operator GLResource<kind>&() {
        return _res;
    }

    GLResource<kind>& get () {
        return _res;
    }
};

template <GLenum kind>
BindGuard<kind> bind_guard (GLuint res, bool alreadyBound = false) { return {res, alreadyBound}; }

template <class T>
BindGuard<T::type> bind_guard (T& res, bool alreadyBound = false) { return {static_cast<GLuint>(res), alreadyBound}; }

template <GLenum kind>
ResourceGuard<kind> resource_guard (GLuint res) { return {res}; }

template <class T>
ResourceGuard<T::type> resource_guard (T& res) { return {static_cast<GLuint>(res)}; }

template <class T>
ResourceGuard<T::type> resource_guard (T&& res) { return {static_cast<GLuint>(res)}; }

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
    // BufferView abstracts over a mapped buffer, allowing for safe and
    // convenient access. Using RAII it guarantees unmapping.
    template <class D, GLenum kind, class T = GLenum>
    class BufferView;

    template <class D, GLenum kind>
    class BufferView<D, kind, traits::IfMappable<kind>>{
    private:
        GLuint _res;
        D* _data;

    public:
        BufferView (GLuint res, GLenum access) :
            _res(res)
        {
            // TODO: Benchmark glMapBuffer vs glMapBufferRange
            detail::GLInterface<kind>::bind(kind,_res);
            _data = static_cast<D*>(glMapBuffer(kind, access));
        }

        BufferView(GLuint res, size_t start, size_t len, GLenum access) :
            _res(res)
        {
            // TODO: Benchmark glMapBuffer vs glMapBufferRange
            detail::GLInterface<kind>::bind(kind,_res);
            _data = static_cast<D*>(glMapBufferRange(kind, start, len, access));
        }


        ~BufferView () {
            commit();
        }

        D& operator[] (size_t idx) {
            return _data[idx];
        }

        void operator= (const D& value) {
            _data[0] = value;
        }

        void write (const D* values, size_t count) {
            for (size_t i = 0; i < count; i++){
                _data[i] = values[i];
            }
        }

        void commit () {
            detail::GLInterface<kind>::bind(kind,_res);
            glUnmapBuffer(kind);
            detail::GLInterface<kind>::bind(kind,0);
            _data = nullptr;
            sglDbgCatchGLError();
        }

    };
} // end namespace

template <class D, class R>
detail::BufferView<D, R::type> buffer_view (R& res, GLenum access = GL_READ_WRITE) {
    return {res,access};
}

template <class D, class R>
detail::BufferView<D, R::type> buffer_view (R&& res, GLenum access = GL_READ_WRITE) {
    return {res,access};
}

template <class D, class R>
detail::BufferView<D, R::type> buffer_view (R& res, size_t start, size_t len, GLenum access) {
    return {res,start,len,access};
}

template <class D, class R>
detail::BufferView<D, R::type> buffer_view (R&& res, size_t start, size_t len, GLenum access) {
    return {res,start,len,access};
}


// GLBuffer is a very thin layer over GLResource that brings type safety to
// OpenGL buffer objects. GLBuffer adds no additional overhead to GLResource.
template <GLenum kind, class T>
class GLBuffer : public GLResource<kind> {
public:
    static_assert(traits::IsBuffer<kind>::value, "Invalid GL Type for buffer");
    using data_type = T;

    GLBuffer () : GLResource<kind>()
    {}

    GLBuffer (GLuint res) : GLResource<kind>(res)
    {}

    GLBuffer (std::vector<T>& data, GLenum usage = GL_DYNAMIC_DRAW) : GLResource<kind>()
    {
        bufferData(*this, data, usage);
    }

    template <size_t len>
    GLBuffer (std::array<T,len>& data, GLenum usage = GL_DYNAMIC_DRAW) : GLResource<kind>()
    {
        bufferData(*this, data, usage);
    }

    GLBuffer (T * data, size_t len, GLenum usage = GL_DYNAMIC_DRAW) : GLResource<kind>()
    {
        bufferData(*this, data, len, usage);
    }

    GLBuffer (T&& data, GLenum usage = GL_DYNAMIC_DRAW ) : GLResource<kind> ()
    {
        bufferData(*this, &data, 1, usage);
    }
};

template <GLenum kind, class T> using MGLBuffer = GLResourceM<GLBuffer<kind,T>>;

template <class T> using ArrayBuffer = GLBuffer<GL_ARRAY_BUFFER, T>;
template <class T> using MArrayBuffer = GLResourceM<ArrayBuffer<T>>;

template <class T = GLuint> using ElementArrayBuffer = GLBuffer<GL_ELEMENT_ARRAY_BUFFER, T>;
template <class T = GLuint> using MElementArrayBuffer = GLResourceM<ElementArrayBuffer<T>>;

template <class T> using UniformBuffer = GLBuffer<GL_UNIFORM_BUFFER, T>;
template <class T> using MUniformBuffer = GLResourceM<UniformBuffer<T>>;

template <class R, class D>
void bufferData (R&& res, std::vector<D>& data, GLenum usage = GL_DYNAMIC_DRAW) {
    static_assert(traits::IsBuffer<R::type>::value, "GLResource target must be buffer");
    bufferData(res,&data[0],data.size(),usage);
}

template <class R, class D, size_t Size>
void bufferData (R&& res, std::array<D,Size>& data, GLenum usage = GL_DYNAMIC_DRAW) {
    static_assert(traits::IsBuffer<R::type>::value, "GLResource target must be buffer");
    bufferData(res,&data[0],Size,usage);
}

template <class R, class D>
void bufferData (R&& res, D* data, size_t len, GLenum usage = GL_DYNAMIC_DRAW){
    static_assert(traits::IsBuffer<R::type>::value, "GLResource target must be buffer");
    detail::GLInterface<R::type>::bind(R::type, static_cast<GLuint>(res));
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


template <GLenum kind, class D>
void bufferData (GLuint res, D*  data, size_t len, GLenum usage = GL_DYNAMIC_DRAW){
    static_assert(traits::IsBuffer<kind>::value, "GLResource target must be buffer");
    detail::GLInterface<kind>::bind(kind, res);
    glBufferData(kind, len * sizeof(D), data, usage);
}


class VertexArray : public GLResource<GL_VERTEX_ARRAY> {
};

using MVertexArray = GLResourceM<VertexArray>;


// VertexAttribBuilder is a helper class that simplifies the
// initialization of Vertex Arrays. At compile time, it computes
// the correct GL types, offsets, strides, and sizes to correctly
// access a buffer in GLSL.
//
// eg:
// The following code creates a VertexArray and creates three attributes
//
//     struct Vertex { float fields[5]; };
//     sgl::ArrayBuffer<Vertex> verts;
//     sgl::VertexArray vao;
//     sgl::vertexAttribBuilder(vao)
//         .addBuffer<float[3], float, float>(verts)
//         .commit();
//

class VertexAttribBuilder {

private:
    GLResource<GL_VERTEX_ARRAY>& vao;

    // Total number of attribs used
    uint32_t attribs;

    // Total number of buffers attached
    uint32_t buffers;

    // Map from buffer to offset / stride to allow for attaching multiple buffers to a VAO
    std::map<GLuint,uint64_t> offsets;
    std::map<GLuint,uint64_t> strides;

public:
    VertexAttribBuilder (GLResource<GL_VERTEX_ARRAY>& vao, uint32_t attribs = 0, uint32_t buffers = 0) :
        vao(vao),
        attribs(attribs),
        buffers(buffers)
    {
        vao.bind();
    }

    void commit () {
        // TODO: Commit implies that the attributes are buffered, and only applied
        // on commit. This might be the behavior later on.
        vao.unbind();
    }

    VertexAttribBuilder& addElementBuffer (GLResource<GL_ELEMENT_ARRAY_BUFFER>& buffer) {
        buffer.bind();
        return *this;
    }

    // TODO: This API currently doesn't support non-normalized buffers.
    // Not sure how to include it in the API.
    template <class T, class ...Ts>
    VertexAttribBuilder& addBuffer (GLResource<GL_ARRAY_BUFFER>& res, GLuint div = 0) {
        strides[res] += traits::param_size<T,Ts...>::size;
        addHelper<T,Ts...>(res, strides[res], div);
        buffers += 1;
        return *this;
    }

    template <class D, class T = D, class ...Ts>
    VertexAttribBuilder& addBuffer (ArrayBuffer<D>& res, GLuint div = 0) {
        strides[res] += traits::param_size<T,Ts...>::size;
        addHelper<T,Ts...>(res, strides[res], div);
        buffers += 1;
        return *this;
    }

private:

    // Private to allow for automatic stride calculation.
    template <class T, class T2, class ...Ts>
    void addHelper (GLResource<GL_ARRAY_BUFFER>& res, int stride, GLuint div, bool normalized = false) {
        addHelper<T>(res,stride,normalized);        // apply attribute
        addHelper<T2,Ts...>(res,stride,normalized); // recurse
    }

    template <class T>
    void addHelper (GLResource<GL_ARRAY_BUFFER>& res, int stride, GLuint div, bool normalized = false) {
        GLenum type = traits::GLType<T>::type;
        size_t elSize = sizeof(typename traits::CType<traits::GLType<T>::type>::type);
        int components = sizeof(T) / elSize;
        res.bind();
        glEnableVertexAttribArray(attribs);
        glVertexAttribPointer(attribs, components, type, SGL_BOOL(normalized), stride, (GLvoid*)offsets[res]);
        glVertexAttribDivisor(attribs, div);
        offsets[res] += sizeof(T);
        attribs += 1;
        sglDbgCatchGLError();
    }
};

inline VertexAttribBuilder vertexAttribBuilder (VertexArray& vao, uint32_t attribs = 0, uint32_t buffers = 0){
    return {vao,attribs,buffers};
}

// Thin wrapper around GL_FRAMEBUFFER. Provides functionality for attaching textures.
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

using MFramebuffer = GLResourceM<Framebuffer>;

using PackBuffer         = GLResource<GL_PIXEL_PACK_BUFFER>;
using UnpackBuffer       = GLResource<GL_PIXEL_UNPACK_BUFFER>;
using QueryBuffer        = GLResource<GL_QUERY_BUFFER>;

using SArrayBuffer        = GLSharedResource<GL_ARRAY_BUFFER>;
using SElementArrayBuffer = GLSharedResource<GL_ELEMENT_ARRAY_BUFFER>;
using SFramebuffer        = GLSharedResource<GL_FRAMEBUFFER>;
using SPackBuffer         = GLSharedResource<GL_PIXEL_PACK_BUFFER>;
using SQueryBuffer        = GLSharedResource<GL_QUERY_BUFFER>;
using SUniformBuffer      = GLSharedResource<GL_UNIFORM_BUFFER>;
using SUnpackBuffer       = GLSharedResource<GL_PIXEL_UNPACK_BUFFER>;
using SVertexArray        = GLSharedResource<GL_VERTEX_ARRAY>;

} // end namespace
