#pragma once

#include "sglconfig.h"
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <map>


namespace sgl {
namespace util {

namespace detail {

    struct SGL_DEBUG_TYPE_STAT {
        size_t count;
        size_t bound;
    };

    struct SGL_DEBUG_STATS_t {
        // Track resource allocation
        std::map<GLenum,SGL_DEBUG_TYPE_STAT> resources;
    };

} // end namespace

const char * glErrorToString (GLenum error);

} // end namespace
} // end namespace

#define SGL_BOOL(v) ((v) ? GL_TRUE : GL_FALSE)

#define sglDbgPrint(...) do { printf("%s:%d ", __FILE__, __LINE__); printf(__VA_ARGS__); } while(0);

#define sglGetGLError(ignore) do {\
       GLenum err = glGetError();\
       if (err != GL_NO_ERROR) {\
            const char * msg = sgl::util::glErrorToString(err);\
            printf("Error code %d at %s:%d: %s\n", err, __FILE__, __LINE__, msg);\
            if (!ignore) exit(EXIT_FAILURE);\
       }\
   } while (0);

#define sglCatchGLError() sglGetGLError(false)
#define sglCheckGLError() sglGetGLError(true)
#define sglClearGLError() glGetError()

#ifdef SGL_DEBUG_STATS
static sgl::util::detail::SGL_DEBUG_STATS_t __sglDebugStats__;
#   define sglDbgRecordCreation(kind,len,dest) do { __sglDebugStats__.resources[kind].count += len; } while(0);
#   define sglDbgRecordDeletion(kind,len,dest) do { __sglDebugStats__.resources[kind].count -= len; } while(0);
#   define sglDbgRecordBind(kind,id)  do { __sglDebugStats__.resources[kind].bound += (id == 0 ? -1 : 1); } while(0);
#   define sglDbgStats() (&__sglDebugStats__)
#else
#   define sglDbgRecordCreation(kind,count,dest)
#   define sglDbgRecordDeletion(kind,count,dest)
#   define sglDbgRecordBind(kind,id)
#   define sglDbgStats() nullptr
#endif

#if SGL_DEBUG > 0
#    define sglDbgCatchGLError() sglGetGLError(false)
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
#else
#   define sglDbgLogVerbose2(...)
#   define sglDbgLogCreation(kind,count,dest) sglDbgRecordCreation(kind,count,dest);
#   define sglDbgLogDeletion(kind,count,dest) sglDbgRecordDeletion(kind,count,dest);
#endif

#if (SGL_DEBUG >= 4)
#   define sglDbgLogVerbose3(...) sglDbgPrint(__VA_ARGS__)
#   define sglDbgLogBind(kind,id) do { sglDbgRecordBind(kind,id); sglDbgPrint("bound resource %d:%d\n",kind,id); } while(0);
#else
#   define sglDbgLogVerbose3(...)
#   define sglDbgLogBind(kind,id) sglDbgRecordBind(kind,id);
#endif
