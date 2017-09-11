#pragma once

#include <type_traits>
#include <array>
#include <stdint.h>

#ifdef SGL_USE_GLM
#    include <glm/glm.hpp>
#endif

namespace sgl {

namespace traits {
    template <bool v>
    struct eval {
        static const bool value = v;
    };

    template <class ...T>
    struct are_same;

    template <class A, class B, class ...T>
    struct are_same<A,B,T...> {
        static const bool value = std::is_same<A,B>::value && are_same<B,T...>::value;
    };

    template <class A>
    struct are_same<A> {
        static const bool value = true;
    };

    template <class T, class ...Ts>
    struct first { typedef T type; };

    template <bool equal, typename... Ts> 
    struct valuetype {};

    template <typename... Ts>
    struct valuetype<true, Ts...> {
        typedef std::array<typename first<Ts...>::type, sizeof...(Ts)> type;
    };

    template <typename... Ts>
    struct valuetype<false, Ts...> {
        typedef std::tuple<Ts...> type;
    };

    template <typename... Ts> 
    struct equal { enum { value = true }; };

    template <typename T> 
    struct equal<T> { enum { value = true }; };


    template <typename T, typename U, typename... Ts> 
    struct equal<T,U,Ts...> {
        enum {
            value = false
        };
    };

    template <std::size_t n>
    struct int2type;

    // one_of checks whether a type T is in a list of other types
    template <class ...T>
    struct one_of;

    template <typename T, typename U, typename... Ts>
    struct one_of<T,U,Ts...> {
        static const bool value = std::is_same<T,U>::value || one_of<T,Ts...>::value;
    };

    template <typename T, typename U>
    struct one_of<T,U> {
        static const bool value = std::is_same<T,U>::value;
    };

    template <typename T>
    struct one_of<T> {
        static const bool value = true;
    };

    // one_of_v checks whether a value of type T is in a list of other values
    template <class T, T ...xs>
    struct one_of_v;

    template <class T, T t, T x, T...xs>
    struct one_of_v<T, t, x, xs...>{
        static const bool value = t == x || one_of_v<T,t,xs...>::value;
    };

    template <class T, T t, T x>
    struct one_of_v<T, t, x> {
        static const bool value = t == x;
    };

    template <class T, T t>
    struct one_of_v<T, t> {
        static const bool value = true;
    };

    template <class T, class ...Ts>
    using enable_if_one = typename std::enable_if<one_of<T,Ts...>::value,T>::type;

    template <class T, T ...Vs>
    using enable_if_one_v = typename std::enable_if<one_of_v<T,Vs...>::value,T>::type;

    template <class T, T v, T hi, T low>
    using in_range = typename std::enable_if<(v >= low && v < hi), T>::type;

    // Useful for optimizing space usage of precomputed lookup tables
    // eg: At compile time the "optimal" representation for "indices" will be chosen
    // template <long items>
    // struct foo {
    //     using type = number_type<items>;
    //     type indices[items];  
    // };

    // For some reason we can't use (1<<32) in templates
    const long _32_bits = 0xffffffff;
    const long _64_bits = 0xffffffffffffffff;

    template <long v, class T = long>
    struct number_type_t;

    template <long v>
    struct number_type_t<v, in_range<long,v,(1<<8),0>> { using type = std::uint8_t; };

    template <long v>
    struct number_type_t<v, in_range<long,v,(1<<16),(1<<8)>> { using type = std::uint16_t; };

    template <long v>
    struct number_type_t<v, in_range<long,v,_32_bits,(1<<16)>> { using type = std::uint32_t; };

    template <long v>
    struct number_type_t<v, in_range<long,v,_64_bits,_32_bits>> { using type = std::uint64_t; };

    template <long v>
    using number_type = typename number_type_t<v>::type;

    template <GLenum v>
    using IsBuffer = traits::one_of_v<GLenum, v,
        GL_ARRAY_BUFFER,
        GL_ELEMENT_ARRAY_BUFFER,
        GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
        GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER,
        GL_QUERY_BUFFER,
        GL_TEXTURE_BUFFER,
        GL_TRANSFORM_FEEDBACK_BUFFER,
        GL_DRAW_INDIRECT_BUFFER,
        GL_ATOMIC_COUNTER_BUFFER,
        GL_DISPATCH_INDIRECT_BUFFER,
        GL_SHADER_STORAGE_BUFFER,
        GL_UNIFORM_BUFFER>;

    template <GLenum v>
    using IfBuffer = typename std::enable_if<traits::IsBuffer<v>::value, GLenum>::type;

    // TODO: Yes, I know this is invalid...
    template <GLenum v>
    using IsVertexArray = traits::one_of_v<GLenum, v, GL_VERTEX_ARRAY>;

    template <GLenum v>
    using IfVertexArray = typename std::enable_if<IsVertexArray<v>::value, GLenum>::type;

    template <GLenum v>
    using IsFramebuffer = traits::one_of_v<GLenum, v,
        GL_FRAMEBUFFER,
        GL_DRAW_FRAMEBUFFER,
        GL_READ_FRAMEBUFFER>;
    
    template <GLenum v>
    using IfFramebuffer = typename std::enable_if<traits::IsFramebuffer<v>::value, GLenum>::type;

    template <GLenum v>
    using IsTexture = traits::one_of_v<GLenum, v,
        GL_TEXTURE_1D, GL_PROXY_TEXTURE_1D,
        GL_TEXTURE_2D, GL_PROXY_TEXTURE_2D,
        GL_TEXTURE_RECTANGLE, GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_3D>;

    template <GLenum v>
    using IfTexture = typename std::enable_if<traits::IsTexture<v>::value, GLenum>::type;

    template <GLenum v>
    using IsTex1D = traits::one_of_v<GLenum, v, GL_TEXTURE_1D, GL_PROXY_TEXTURE_1D>;

    template <GLenum v>
    using IfTex1D = typename std::enable_if<traits::IsTex1D<v>::value, GLenum>::type;

    template <GLenum v>
    using IsTex2D = traits::one_of_v<GLenum, v, GL_TEXTURE_2D, GL_PROXY_TEXTURE_2D, GL_TEXTURE_RECTANGLE>;

    template <GLenum v>
    using IfTex2D = typename std::enable_if<traits::IsTex2D<v>::value, GLenum>::type;
    
    template <GLenum V>
    using IsTex2DArray = traits::one_of_v<GLenum, V, GL_TEXTURE_CUBE_MAP>;

    template <GLenum v>
    using IfTex2DArray = typename std::enable_if<traits::IsTex2DArray<v>::value, GLenum>::type;

    template <GLenum V>
    using IsTex3D = traits::one_of_v<GLenum, V, GL_TEXTURE_3D>;

    template <GLenum v>
    using IfTex3D = typename std::enable_if<traits::IsTex3D<v>::value, GLenum>::type;

    template <GLenum V>
    using IsWritable = traits::eval<IsBuffer<V>::value || IsTexture<V>::value>;

    template <GLenum v>
    using IfWritable = typename std::enable_if<traits::IsWritable<v>::value, GLenum>::type;

    template <GLenum v>
    using IsMappable = traits::eval<traits::IsBuffer<v>::value>;

    template <GLenum v>
    using IfMappable = typename std::enable_if<traits::IsMappable<v>::value, GLenum>::type;

    template <GLenum v>
    using IsShaderProgram = traits::one_of_v<GLenum, v, GL_PROGRAM>;

    template <GLenum v>
    using IfShaderProgram = typename std::enable_if<traits::IsShaderProgram<v>::value, GLenum>::type;


    // C++ Type to GL Type
    template <class T>
    struct GLType {
        const static GLenum type;
    };

    template<> struct GLType<uint8_t> { const static GLenum type = GL_UNSIGNED_BYTE; };
    template<> struct GLType<int8_t> { const static GLenum type = GL_BYTE; };
    template<> struct GLType<char> { const static GLenum type = GL_BYTE; };
    template<> struct GLType<uint16_t> { const static GLenum type = GL_UNSIGNED_SHORT; };
    template<> struct GLType<short> { const static GLenum type = GL_SHORT; };
    template<> struct GLType<unsigned int> { const static GLenum type = GL_UNSIGNED_INT; };
    template<> struct GLType<int> { const static GLenum type = GL_INT; };
    template<> struct GLType<double> { const static GLenum type = GL_DOUBLE; };
    template<> struct GLType<float> { const static GLenum type = GL_FLOAT; };

#ifdef SGL_USE_GLM  
    template<> struct GLType<glm::vec2> { const static GLenum type = GL_FLOAT; };
    template<> struct GLType<glm::vec3> { const static GLenum type = GL_FLOAT; };
    template<> struct GLType<glm::vec4> { const static GLenum type = GL_FLOAT; };
    template<> struct GLType<glm::mat3> { const static GLenum type = GL_FLOAT; };
    template<> struct GLType<glm::mat4> { const static GLenum type = GL_FLOAT; };
#endif

    // GL Type to C++ type
    template <GLenum T>
    struct CType;

    template<> struct CType<GL_UNSIGNED_BYTE> { using type = uint8_t; };
    template<> struct CType<GL_BYTE> { using type = char; };
    template<> struct CType<GL_UNSIGNED_SHORT> { using type = uint16_t; };
    template<> struct CType<GL_SHORT> { using type = short; };
    template<> struct CType<GL_UNSIGNED_INT> { using type = unsigned int; };
    template<> struct CType<GL_INT> { using type = int; };
    template<> struct CType<GL_DOUBLE> { using type = double; };
    template<> struct CType<GL_FLOAT> { using type = float; };

} // namespace
} // namespace

