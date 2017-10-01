#ifndef RESOURCE2_H
#define RESOURCE2_H

#include "sglconfig.h"
#include "utils.h"
#include "traits.h"

#include <stdint.h>
#include <set>
#include <map>
#include <vector>
#include <array>

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

    template <GLenum kind, class T = GLenum>
    struct GLInterface {};

    template <GLenum kind>
    struct GLInterface<kind, traits::IfBuffer<kind>> {
        static void create (int len, GLuint* dest) {  __glewGenBuffers(len,dest); sglDbgLogCreation(kind,len,dest);  }
        static void destroy (int len, GLuint* dest) { __glewDeleteBuffers(len,dest); sglDbgLogDeletion(kind,len,dest);  }
        static void bind (GLuint id) { __glewBindBuffer(kind,id); sglDbgLogBind(kind,id); }
    };

    template <GLenum kind>
    struct GLInterface<kind, traits::IfFramebuffer<kind>> {
        static void create (int len, GLuint* dest) { __glewGenFramebuffers(len,dest); sglDbgLogCreation(kind,len,dest);}
        static void destroy (int len, GLuint* dest) { __glewDeleteFramebuffers(len,dest); sglDbgLogDeletion(kind,len,dest);}
        static void bind (GLuint id) { __glewBindFramebuffer(kind,id); sglDbgLogBind(kind,id);}
    };

    template <GLenum kind>
    struct GLInterface<kind, traits::IfTexture<kind>> {
        static void create (int len, GLuint* dest) { glGenTextures(len,dest); sglDbgLogCreation(kind,len,dest);}
        static void destroy (int len, GLuint* dest) { glDeleteTextures(len,dest); sglDbgLogDeletion(kind,len,dest);}
        static void bind (GLuint id) { glBindTexture(kind,id); sglDbgLogBind(kind,id);}
    };

    template <GLenum kind>
    struct GLInterface<kind, traits::IfVertexArray<kind>> {
        static void create (int len, GLuint* dest) { __glewGenVertexArrays(len,dest); sglDbgLogCreation(kind,len,dest);}
        static void destroy (int len, GLuint* dest) { __glewDeleteVertexArrays(len,dest); sglDbgLogDeletion(kind,len,dest);}
        static void bind (GLuint id) { __glewBindVertexArray(id); sglDbgLogBind(kind,id);}
    };

    template <GLenum kind>
    struct GLInterface<kind, traits::IfShaderStage<kind>> {
        static void create (int len, GLuint* dest) { *dest = glCreateShader(kind); sglDbgLogCreation(kind,len,dest);}
        static void destroy (int len, GLuint* dest) { glDeleteShader(*dest); sglDbgLogDeletion(kind,len,dest);}
        static void bind (GLuint id) {}
    };

    template <GLenum kind>
    struct GLInterface<kind, traits::IfShaderProgram<kind>> {
        static void create (int len, GLuint* dest) { __glCreateProgram(len,dest); sglDbgLogCreation(kind,len,dest);}
        static void destroy (int len, GLuint* dest) { __glDeleteProgram(len,dest); sglDbgLogDeletion(kind,len,dest);}
        static void bind (GLuint id) { __glUseProgram(kind,id); sglDbgLogBind(kind,id);}
    };

#if SGL_RENDERBUFFER_SUPPORTED
    template <>
    struct GLInterface<GL_RENDERBUFFER, GLenum>{
        static void create (int len, GLuint* dest) { glGenRenderbuffers(len,dest); sglDbgLogCreation(kind,len,dest); }
        static void destroy (int len, GLuint* dest) { glDeleteRenderbuffers(len,dest); sglDbgLogDeletion(kind,len,dest); }
        static void bind (GLuint id) { glBindRenderbuffer(kind,id); sglDbgLogBind(kind,id); }
    };
#endif // SGL_RENDERBUFFER_SUPPORTED


#ifdef SGL_BUFFERSTORAGE_SUPPORTED
    static GLbitfield SGL_RW = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
    using UsageType = GLbitfield;
#else
    static GLenum SGL_RW = GL_READ_WRITE;
    using UsageType = GLenum;
#endif // SGL_BUFFERSTORAGE_SUPPORTED

    template <GLenum kind, class T = GLenum>
    struct GLBufferInterface;

    template <GLenum kind>
    struct GLBufferInterface<kind, traits::IfBuffer<kind>> {
        void initialize (GLuint res, const char * data, size_t len, UsageType usage) {
            #ifdef SGL_BUFFERSTORAGE_SUPPORTED
                    glBufferStorage(kind, len, data, usage);
            #else
                    glBufferData(kind,len,data,usage);
            #endif // SGL_BUFFERSTORAGE_SUPPORTED
            sglDbgLogVerbose("Initializing immutable buffer %d:%d size: %lu", kind, res, len);
        }

        void initializeMut (GLuint res, const char * data, size_t len, GLenum usage) {
            glBufferData(kind,len,data,usage);
            sglDbgLogVerbose("Initializing mutable buffer %d:%d size: %lu", kind, res, len);
        }

        void update (GLuint res, const char * data, size_t start, size_t len) {
            glBufferSubData(kind,start,len,data);
            sglDbgLogVerbose("Writing range %lu:%lu bytes to %d:%d", start, start+len, kind, res);
        }

    };
#ifdef SGL_RENDERBUFFER_SUPPORTED
    template <GLenum kind>
    struct GLBufferInterface<kind, traits::IfRenderBuffer<kind>> {
    };
#endif // SGL_RENDERBUFFER_SUPPORTED




} // end namespace

template <GLenum kind>
class GLResource {
protected:
    GLuint _id;

public:
    static const GLenum type = kind;

    GLResource ()  {
        detail::GLInterface<kind>::create(1, &_id);
    }

    GLResource (GLuint res) :
        _id(res)
    {}

    operator GLuint() const { return _id; }

    void bind () {
        detail::GLInterface<kind>::bind(_id);
    }

    void unbind () {
        detail::GLInterface<kind>::bind(0);
    }

    void release () {
        detail::GLInterface<kind>::destroy(1, &_id);
    }
};

template <GLenum kind>
inline void bind (GLuint res) {
    detail::GLInterface<kind>::bind(res);
}

template <GLenum kind>
inline GLuint create () {
    GLuint dest;
    detail::GLInterface<kind>::create(1,&dest);
    return dest;
}

template <GLenum kind>
inline void create (GLuint * dest, size_t len) {
    detail::GLInterface<kind>::create(len,dest);
}

template <GLenum kind>
inline void destroy (GLuint res) {
    detail::GLInterface<kind>::destroy(1,&res);
}

template <GLenum kind>
inline void destroy (GLuint* dest, size_t len) {
    detail::GLInterface<kind>::destroy(len,&dest);
}


template <GLenum kind>
class GLResourceArray {
protected:
    GLuint * ids;

public:
    size_t size;
    static const GLenum type = kind;

    GLResourceArray (GLuint * ids, size_t size) :
        ids(ids),
        size(size)
    {}

    GLResourceArray (size_t size) :
        size(size)
    {
        ids = new GLuint[size];
        detail::GLInterface<kind>::create(size,ids);
    }

    ~GLResourceArray () {
        release();
    }

    GLResource<kind> operator[] (size_t idx) {
        return ids[idx];
    }

    void bind (size_t idx) {
        detail::GLInterface<kind>::bind(ids[idx]);
    }

    void unbind (size_t idx) {
        detail::GLInterface<kind>::bind(0);
    }

    void release () {
        detail::GLInterface<kind>::destroy(size,ids);
        delete ids; // TODO: Because the user can wrap their own ptr, this might not be a good idea...
        ids = nullptr;
    }
};

template <GLenum kind>
class BindGuard {
private:
    // Resource instead of GLuint because we may have overriden bind to bind child resources
    // GLuint -> GLResource should have zero overhead.
    GLResource<kind> _res;

    // TODO: A bit of a hack to cut down on unecessary binds
    // We can't just guard the creation of the guard in an if block because
    // it will be destroyed as a result.
    bool _alreadyBound;

public:
    BindGuard (GLuint res, bool alreadyBound = false) :
        _res(res),
        _alreadyBound(alreadyBound)
    {
        if (!_alreadyBound) _res.bind();
    }

    BindGuard (GLResource<kind>& res, bool alreadyBound = false) :
        _res(res),
        _alreadyBound(alreadyBound)
    {
        if (!_alreadyBound) _res.bind();

    }

    ~BindGuard () {
        if (!_alreadyBound) _res.unbind();
    }
};

// Helpers for creating BindGuard. Allows for use of auto keyword.
//
// ex:
//     auto bg_nice = sgl::bind_guard(res);
//     sgl::BindGuard<decltype(res)::type> bg_ugly{res};
template <GLenum kind>
BindGuard<kind> bind_guard (GLuint res, bool alreadyBound = false) {
    return {res,alreadyBound};
}

template <GLenum kind>
BindGuard<kind> bind_guard (GLResource<kind>& res, bool alreadyBound = false) {
    return {res,alreadyBound};
}

template <GLenum kind>
BindGuard<kind> bind_guard (GLResource<kind>&& res, bool alreadyBound = false) {
    return {res,alreadyBound};
}

template <GLenum kind>
class ResourceGuard {
private:
    // Resource instead of GLuint because we may have overriden release to release child resources
    // GLuint -> GLResource should have zero overhead.
    GLResource<kind> _res;

public:
    ResourceGuard (GLResource<kind>& res) :
        _res(res)
    {}

    ~ResourceGuard () {
        _res.release();
    }

    operator GLResource<kind>() {
        return _res;
    }

    GLResource<kind>& get() {
        return _res;
    }
};

// Helpers for creating ResourceGuard. Allows for use of auto keyword.
//
// ex:
//     auto rg_nice = sgl::resource_guard(res);
//     sgl::ResourceGuard<decltype(res)::type> rg_ugly{res};
template <GLenum kind>
ResourceGuard<kind> resource_guard (GLuint res) { return {res}; }

template <GLenum kind>
ResourceGuard<kind> resource_guard (GLResource<kind>& res) { return {res}; }

template <GLenum kind>
ResourceGuard<kind> resource_guard (GLResource<kind>&& res) { return {res}; }

// TODO: decide whether to keep track of buffer size. This could help
// make code cleaner and mode concise, as well as potentially allowing
// for some optimizations. For example, if we knew the size we could
// choose between glBufferData and glBufferSubData depending on
// the size of the input array. The drawback however is that
// we'll be adding between 4 and 8 bytes more per buffer. Statically
// declaring the buffer size isn't much of a solution either as it
// will make the API needlessly clunky.
template <GLenum kind, class T>
class GLBuffer : public GLResource<kind> {
public:
    using value_type = T;

    GLBuffer (GLuint res) : GLResource<kind>(res) {
    }

    GLBuffer () : GLResource<kind>() {
    }

    GLBuffer (const std::vector<T>& data, detail::UsageType flags = detail::SGL_RW) :
        GLResource<kind>()
    {
        detail::GLBufferInterface<kind>::initialize(this->_id, reinterpret_cast<const char*>(&data[0]), sizeof(T) * data.size(), flags);
    }

    template <size_t len>
    GLBuffer (const std::array<T,len>& data, detail::UsageType flags = detail::SGL_RW) :
        GLResource<kind>()
    {
        detail::GLBufferInterface<kind>::initialize(this->_id, reinterpret_cast<const char*>(&data[0]), sizeof(T) * len, flags);
    }

    GLBuffer (const T* data, size_t len, detail::UsageType flags = detail::SGL_RW) :
        GLResource<kind>()
    {
        detail::GLBufferInterface<kind>::initialize(this->_id, reinterpret_cast<const char*>(data), sizeof(T) * len, flags);
    }

    void reserve (size_t count, detail::UsageType flags = detail::SGL_RW) {
        detail::GLBufferInterface<kind>::initialize(this->_id, NULL, sizeof(T) * count, flags);
    }
};

template <GLenum kind, class T>
class GLBufferMut : public GLResource<kind> {
public:
    using value_type = T;

    GLBufferMut (GLuint res) : GLResource<kind>(res) {
    }

    GLBufferMut () : GLResource<kind>() {
    }

    GLBufferMut (const std::vector<T>& data, GLenum usage = GL_READ_WRITE) :
        GLResource<kind>()
    {
        detail::GLBufferInterface<kind>::initializeMut(this->_id, reinterpret_cast<const char*>(&data[0]), sizeof(T) * data.size(), usage);
    }

    template <size_t len>
    GLBufferMut (const std::array<T,len>& data, GLenum usage = GL_READ_WRITE) :
        GLResource<kind>()
    {
        detail::GLBufferInterface<kind>::initializeMut(this->_id, reinterpret_cast<const char*>(&data[0]), sizeof(T) * len, usage);
    }

    GLBufferMut (const T* data, size_t len, GLenum usage = GL_READ_WRITE) :
        GLResource<kind>()
    {
        detail::GLBufferInterface<kind>::initializeMut(this->_id, reinterpret_cast<const char*>(data), sizeof(T) * len, usage);
    }

    void reserve (size_t count, GLenum usage = GL_READ_WRITE) {
        detail::GLBufferInterface<kind>::initializeMut(this->_id, NULL, sizeof(T) * count, usage);
    }

    void resize (size_t count, GLenum usage = GL_READ_WRITE) {
        detail::GLBufferInterface<kind>::initializeMut(this->_id, NULL, sizeof(T) * count, usage);
    }
};

using PackBuffer         = GLResource<GL_PIXEL_PACK_BUFFER>;
using UnpackBuffer       = GLResource<GL_PIXEL_UNPACK_BUFFER>;
using QueryBuffer        = GLResource<GL_QUERY_BUFFER>;

template <class T>
using UniformBuffer      = GLBuffer<GL_UNIFORM_BUFFER, T>;

using RenderBuffer       = GLResource<GL_RENDERBUFFER>;

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
            sglDbgLogVerbose("Mapping buffer: %d:%d\n", kind, _res);
            sgl::bind<kind>(_res);
            _data = static_cast<D*>(glMapBuffer(kind, access));
        }

        BufferView(GLuint res, size_t start, size_t len, GLenum access) :
            _res(res)
        {
            // TODO: Benchmark glMapBuffer vs glMapBufferRange
            sglDbgLogVerbose("Mapping buffer: %d:%d\n", kind, _res);
            detail::GLInterface<kind>::bind(kind,_res);
            _data = static_cast<D*>(glMapBufferRange(kind, start, len, access));
            sglDbgCatchGLError();
        }


        ~BufferView () {
            if (_data != nullptr) commit();
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
            sgl::bind<kind>(_res);
            sglDbgLogVerbose("Unmapping buffer %d:%d\n", kind, _res);
            glUnmapBuffer(kind);
            sglDbgCatchGLError();
            sgl::bind<kind>(0);
            sglDbgCatchGLError();
            _data = nullptr;
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

template <class R, class D>
void bufferData (R&& res, std::vector<D>& data, GLenum usage = GL_DYNAMIC_DRAW) {
    using M = typename std::remove_reference<R>::type;
    static_assert(traits::IsBuffer<M::type>::value, "GLResource target must be buffer");
    bufferData(res,&data[0],data.size(),usage);
}

template <class R, class D, size_t Size>
void bufferData (R&& res, std::array<D,Size>& data, GLenum usage = GL_DYNAMIC_DRAW) {
    using M = typename std::remove_reference<R>::type;
    static_assert(traits::IsBuffer<M::type>::value, "GLResource target must be buffer");
    bufferData(res,&data[0],Size,usage);
}

template <class R, class D>
void bufferData (R&& res, D* data, size_t len, GLenum usage = GL_DYNAMIC_DRAW){
    using M = typename std::remove_reference<R>::type;
    static_assert(traits::IsBuffer<M::type>::value, "GLResource target must be buffer");
    detail::GLInterface<M::type>::bind(static_cast<GLuint>(res));
    glBufferData(M::type, len * sizeof(D), data, usage);
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
    detail::GLInterface<kind>::bind(res);
    glBufferData(kind, len * sizeof(D), data, usage);
}

template <class T> using ArrayBuffer    = GLBuffer<GL_ARRAY_BUFFER,T>;
template <class T> using ArrayBufferMut = GLBufferMut<GL_ARRAY_BUFFER,T>;

// TODO: Does this need to be anything other than uint32_t?
template <class T = uint32_t>
using ElementArrayBuffer    = GLBuffer<GL_ELEMENT_ARRAY_BUFFER, T>;

template <class T = uint32_t>
using ElementArrayBufferMut = GLBufferMut<GL_ELEMENT_ARRAY_BUFFER, T>;

#if SGL_VERTEXARRAY_SUPPORTED
using VertexArray = GLResource<GL_VERTEX_ARRAY>;


/**
* VertexAttribBuilder is a helper class that simplifies the
* initialization of Vertex Arrays. At compile time, it computes
* the correct GL types, offsets, strides, and sizes to correctly
* access a buffer in GLSL.
*
* eg:
* The following code creates a VertexArray and creates three attributes
*
*     struct Vertex { float fields[5]; };
*     sgl::ArrayBuffer<Vertex> verts;
*     sgl::VertexArray vao;
*     sgl::vertexAttribBuilder(vao)
*         .addBuffer<float[3], float, float>(verts)
*         .commit();
*
* eg:
* Non interleaved attributes
*
*     sgl::ArrayBuffer<float> vertBuf{verts};
*     sgl::ArrayBuffer<float> uvBuf{uvs};
*     sgl::ElementArrayBuffer<> elBuf{idxs};
*     sgl::VertexArray vao;
*     sgl::vertexAttribBuilder(vao)
*         .addElementBuffer(elBuf)
*         .addBuffer<sgl::vec3f>(vertBuf)
*         .addBuffer<sgl::vec2f>(uvBuf)
*         .commit();
*
* eg:
* Optional attributes
*
*     sgl::ArrayBuffer<float> vertBuf{verts};
*     sgl::VertexArray vao;
*
*     sgl::VertexAttribBuilder builder(vao);
*     builder.addBuffer<sgl::vec3f>(vertBuf);
*     if (hasUVs)   builder.addBuffer<sgl::vec2f>(vertBuf);
*     else          builder.skip<sgl::vec2f>(vertBuf);
*     if (hasNorms) builder.addBuffer<sgl::vec3f>(vertBuf);
*     else          builder.skip<sgl::vec3f>(vertBuf);
*     if (hasColor) builder.addBuffer<sgl::vec4f>(vertBuf);
*     else          builder.skip<sgl::vec4f>(vertBuf);
*     builder.commit();
*/

namespace detail {
    struct VertexAttrib {
        GLenum type;
        size_t offset;
        size_t elSize;
        size_t components;
        GLuint div;
        uint32_t attrib;
    };

    struct VertexAttribNew {
        GLenum type;
        size_t offset;
        size_t elSize;
        size_t components;
        GLuint div;
    };

    struct VertexAttribBuffer {
        size_t stride;
        size_t offset;
        std::vector<VertexAttribNew> attribs;
    };

    template <class T>
    VertexAttribNew makeVertexAttrib (size_t offset, GLuint div = 0) {
        GLenum type = traits::GLType<T>::type;
        size_t elSize = sizeof(typename traits::CType<traits::GLType<T>::type>::type);
        size_t components = sizeof(T) / elSize;
        return { type, offset, elSize, components, div };
    }
} // end namespace

// TODO: This approach makes it impossible to
// skip elements of a interleaved vbo. Current work around
// is to add a skip function, which isn't pretty, but it works...

// TODO: Rework this to use VertexAttribNew and VertexAttribBuffer. It's
// more elegant, more flexible, and more performant.
class VertexAttribBuilder {
private:
    VertexArray& vao;

    // Total number of attribs used
    uint32_t attribs;

    // Map from buffer to offset / stride to allow for attaching multiple buffers to a VAO
    std::map<GLuint,GLint> strides;
    std::map<GLuint,uint64_t> offsets;
    std::map<GLuint,std::vector<detail::VertexAttrib>> attribQueues;
    std::set<GLuint> buffers;
    GLuint ebo = 0;

public:
    VertexAttribBuilder (VertexArray& vao, uint32_t attribs = 0) :
        vao(vao),
        attribs(attribs)
    {}

    /**
    * @brief commit Perform vertex array configuration.
    */
    void commit () {
        vao.bind();
        if (ebo != 0) sgl::bind<GL_ELEMENT_ARRAY_BUFFER>(ebo);
        for (const auto& b : buffers) {
            sgl::bind<GL_ARRAY_BUFFER>(b);
            for (const auto& attr : attribQueues[b]){
                addAttribute(attr, strides[b]);
            }
        }
        vao.unbind();
    }

    /**
     * @brief addElementBuffer Set VAO's element array buffer
     * @param buffer
     * @return VertexAttribBuilder refernece
     */
    VertexAttribBuilder& addElementBuffer (GLResource<GL_ELEMENT_ARRAY_BUFFER>& buffer) {
        ebo = buffer;
        return *this;
    }



    // TODO: This API currently doesn't support non-normalized buffers.
    // Not sure how to include it in the API.


    template <class T, class ...Ts>
    VertexAttribBuilder& addBuffer (GLResource<GL_ARRAY_BUFFER>& res, GLuint div = 0) {
        strides[res] += traits::param_size<T,Ts...>::size;
        buffers.insert(res);
        queueAttrib<T,Ts...>(res, div);
        return *this;
    }

    template <class D, class T = D, class ...Ts>
    VertexAttribBuilder& addBuffer (ArrayBuffer<D>& res, GLuint div = 0) {
        strides[res] += traits::param_size<T,Ts...>::size;
        buffers.insert(res);
        queueAttrib<D,T,Ts...>(res, div);
        return *this;
    }

    // TODO: Hack to support skipping vertices in interleaved vertex buffer.
    // Rework the interface a bit to add a more expressive, flexible data type
    // that allows the user to manually specify everything.
    template <class T>
    VertexAttribBuilder& skip (GLResource<GL_ARRAY_BUFFER>& res) {
        strides[res] += sizeof(T);
        offsets[res] += sizeof(T);
        return *this;
    }

    VertexAttribBuilder& reset () {
        buffers.clear();
        strides.empty();
        offsets.empty();
        attribQueues.empty();
        ebo = 0;
        attribs = 0;
        return *this;
    }

private:

    template <class T, class T2, class ...Ts>
    void queueAttrib (GLResource<GL_ARRAY_BUFFER>& res, GLuint div = 0) {
        queueAttrib<T>(res,div);
        queueAttrib<T2,Ts...>(res,div);
    }

    template <class T>
    void queueAttrib (GLResource<GL_ARRAY_BUFFER>& res, GLuint div = 0) {
        GLenum type = traits::GLType<T>::type;
        size_t elSize = sizeof(typename traits::CType<traits::GLType<T>::type>::type);
        size_t components = sizeof(T) / elSize;
        attribQueues[res].push_back({
            type, offsets[res], elSize, components, div, attribs
        });
        offsets[res] += sizeof(T);
        attribs += 1;
    }

    void addAttribute (const detail::VertexAttrib& attrib, GLint stride) {
        glEnableVertexAttribArray(attrib.attrib);
        glVertexAttribPointer(attrib.attrib, attrib.components, attrib.type, GL_FALSE, stride, (GLvoid*)attrib.offset);
        glVertexAttribDivisor(attrib.attrib, attrib.div);
        sglDbgCatchGLError();
    }
};

inline VertexAttribBuilder vertexAttribBuilder (VertexArray& vao, uint32_t attribs = 0){
    return {vao,attribs};
}
#endif // SGL_VERTEXARRAY_SUPPORTED

#if SGL_FRAMEBUFFER_SUPPORTED
// Thin wrapper around GL_FRAMEBUFFER. Provides functionality for attaching textures and renderbuffers
template <GLenum kind>
class Framebuffer : public GLResource<kind> {
    static_assert(traits::IsFramebuffer<kind>::value, "Not valid framebuffer target");
public:
    Framebuffer () :
        GLResource<kind>()
    {}

    template <GLenum res>
    traits::IfTex1D<res,void> attachTexture (GLResource<res>& texture, GLenum attachment = GL_COLOR_ATTACHMENT0) {
        this->bind();
        glFramebufferTexture1D(kind, attachment, res, (GLuint)texture, 0);
    }

    template <GLenum res>
    traits::IfTex2D<res,void> attachTexture (GLResource<res>& texture, GLenum attachment = GL_COLOR_ATTACHMENT0) {
        this->bind();
        glFramebufferTexture2D(kind, attachment, res, (GLuint)texture, 0);
    }

#ifdef SGL_RENDERBUFFER_SUPPORTED
    void attachRenderBuffer (RenderBuffer& buffer, GLenum attachment = GL_COLOR_ATTACHMENT0) {
        this->bind();
        glFramebufferRenderbuffer(kind, attachment, buffer.type, buffer);
    }
#endif // SGL_RENDERBUFFER_SUPPORTED
};
#endif // SGL_FRAMEBUFFER_SUPPORTED


} // end namespace

#endif // RESOURCE2_H
