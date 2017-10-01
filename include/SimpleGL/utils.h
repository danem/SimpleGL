#pragma once

#include "sglconfig.h"
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <map>

#define _sglGetGLError(ignore) do {\
       GLenum err = glGetError();\
       if (err != GL_NO_ERROR) {\
            const char * msg = sgl::util::glErrorToString(err);\
            printf("Error code %d at %s:%d: %s\n", err, __FILE__, __LINE__, msg);\
            if (!ignore) exit(EXIT_FAILURE);\
       }\
   } while (0);

#define sglCatchGLError() _sglGetGLError(false)
#define sglCheckGLError() _sglGetGLError(true)
#define sglClearGLError() glGetError()
#define SGL_BOOL(v) ((v) ? GL_TRUE : GL_FALSE)

#define sglDbgPrint(...) do { printf("%s:%d ", __FILE__, __LINE__); printf(__VA_ARGS__); } while(0);

#ifdef SGL_DEBUG_STATS
sgl::util::detail::SGL_DEBUG_STATS __sglDebugStats__;
#   define sglDbgRecordCreation(kind,count,dest) do { __sglDebugStats__.resources[kind].count += count; } while(0);
#   define sglDbgRecordDeletion(kind,count,dest) do { __sglDebugStats__.resources[kind].count -= count; } while(0);
#   define sglDbgRecordBind(kind,id)  do { __sglDebugStats__.resources[kind].bound += (id == 0 ? -1 : 1); } while(0);
#else
#   define sglDbgRecordCreation(kind,count,dest)
#   define sglDbgRecordDeletion(kind,count,dest)
#   define sglDbgRecordBind(kind,id)
#endif

#ifdef SGL_DEBUG
#    define sglDbgCatchGLError() _sglGetGLError(false)
#    define sglDgbLog(...) sglDbgPrint(__VA_ARGS__)
#else
#    define sglDbgCatchGLError()
#    define sglDbgLog(...)
#endif

#if (SGL_DEBUG >= 1)
#   define sglDbgLogInfo(...) sglDbgPrint(__VA_ARGS__)
#else
#   define sglDbgLogInfo(...)
#endif

#if (SGL_DEBUG >= 2)
#   define sglDbgLogVerbose(...) sglDbgPrint(__VA_ARGS__)
#else
#   define sglDbgLogVerbose(...)
#endif

#if (SGL_DEBUG >= 3)
#   define sglDbgLogVerbose2(...) sglDbgPrint(__VA_ARGS__)
#   define sglDbgLogCreation(kind,count,dest) do { sglDbgRecordCreation(kind,count,dest); for(int i=0;i<count;i++){ sglDbgPrint("created resource %d:%d\n",kind,dest[i]); }} while(0);
#   define sglDbgLogDeletion(kind,count,dest) do { sglDbgRecordDeletion(kind,count,dest); for(int i=0;i<count;i++){ sglDbgPrint("deleted resource %d:%d\n",kind,dest[i]); }} while(0);
#   define sglDbgLogBind(kind,id) do { sglDbgRecordBind(kind,id); sglDbgPrint("bound resource %d:%d\n",kind,id); } while(0);
#else
#   define sglDbgLogVerbose2(...)
#   define sglDbgLogCreation(kind,count,dest) sglDbgRecordCreation(kind,count,dest);
#   define sglDbgLogDeletion(kind,count,dest) sglDbgRecordDeletion(kind,count,dest);
#   define sglDbgLogBind(kind,id) sglDbgRecordBind(kind,id);
#endif


namespace sgl {
namespace util {

const char * glErrorToString (GLenum error);

class Formatter {
private:
    std::stringstream _stream;

public:
    Formatter () {}
    ~Formatter() {}

    template <class T>
    Formatter& operator<< (const T& value) {
        _stream << value;
        return *this;
    }

    std::string str() const;
    operator std::string () const;
};

template <class T>
class Range {
public:
    T low;
    T high;

    Range (T high) : high(high), low(0){}
    Range (T high, T low) : high(high), low(low) {}
};

namespace detail {

    struct SGL_DEBUG_TYPE_STAT {
        size_t count;
        size_t bound;
    };

    struct SGL_DEBUG_STATS {
        // Track resource allocation
        std::map<GLuint,SGL_DEBUG_TYPE_STAT> resources;
    };

} // end namespace


} // end namespace
} // end namespace
