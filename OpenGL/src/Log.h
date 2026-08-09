#include <GL/glew.h>
#include <iostream>

// __debugbreak() is an MSVC intrinsic and does not exist on clang or gcc, so
// every GLCall failed to compile on macOS and Linux. MSVC keeps the original
// intrinsic; the others get their nearest equivalent.
#if defined(_MSC_VER)
    #define DEBUG_BREAK() __debugbreak()
#elif defined(__clang__)
    #define DEBUG_BREAK() __builtin_debugtrap()
#elif defined(__GNUC__)
    #include <csignal>
    #define DEBUG_BREAK() raise(SIGTRAP)
#else
    #define DEBUG_BREAK() ((void)0)
#endif

#define ASSERT(x) if (!(x)) DEBUG_BREAK();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLCheckError();
void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);
