#pragma once

// Platform.h - portability shims added when the project gained macOS/Linux builds.
//
// The simulation itself is untouched. Everything here either compiles to the
// pre-existing Windows behaviour verbatim, or supplies the platform plumbing
// that non-Windows targets require in order to reach the same behaviour.

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Platform {

// ---------------------------------------------------------------------------
// Deterministic RNG
//
// SandBox seeds the 500x500 grid from rand() and never calls srand(), so the
// starting world is whatever the C runtime's generator produces from seed 1.
// That generator is not specified by the standard: MSVC, glibc and libc++ each
// use a different one, and RAND_MAX differs too (32767 vs 2147483647). Left as
// std::rand(), a macOS or Linux build would boot into a completely different
// world than the Windows build.
//
// This is MSVC's generator, reproduced exactly. On Windows it is a no-op --
// verified against std::rand() over 250,000 draws with zero mismatches
// (41, 18467, 6334, 26500, 19169, ...). On every other platform it makes the
// starting world identical to Windows.
// ---------------------------------------------------------------------------

#define PLATFORM_RAND_MAX 0x7fff

inline unsigned long& RandState() {
    static unsigned long state = 1UL;
    return state;
}

inline int Rand() {
    unsigned long& s = RandState();
    s = s * 214013UL + 2531011UL;
    return (int)((s >> 16) & 0x7fff);
}

// ---------------------------------------------------------------------------
// GL context creation
//
// macOS only exposes OpenGL beyond 2.1 through a forward-compatible core
// profile, which has to be requested before the window is created. Without
// these hints the "#version 330 core" shaders fail to compile there.
// Windows and Linux keep the original default context, so their behaviour is
// unchanged.
// ---------------------------------------------------------------------------

inline void ApplyGLWindowHints() {
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

// GLEW needs glewExperimental on a core profile or it skips entry points and
// leaves them null. Only the platforms that request a core profile need it.
inline bool InitGLLoader() {
#ifdef __APPLE__
    glewExperimental = GL_TRUE;
#endif
    return glewInit() == GLEW_OK;
}

} // namespace Platform
