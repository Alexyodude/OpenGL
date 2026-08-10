cmake_minimum_required(VERSION 3.16)
project(OpenGL LANGUAGES CXX)

# Cross-platform build for the mass/wave sandbox simulation.
#
# The original project builds from OpenGL/OpenGL.vcxproj, which only MSBuild
# understands. This file compiles the exact same translation units on Windows,
# macOS and Linux so the simulation can ship on all three.
#
#   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
#   cmake --build build --config Release

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/src")
set(DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/Dependices")

# Same ClCompile set as OpenGL.vcxproj.
set(SOURCES
    ${SRC_DIR}/Source.cpp
    ${SRC_DIR}/SandBox.cpp
    ${SRC_DIR}/Layer.cpp
    ${SRC_DIR}/Log.cpp
    ${SRC_DIR}/Player.cpp
    ${SRC_DIR}/Renderer.cpp
    ${SRC_DIR}/Shader.cpp
    ${SRC_DIR}/IndexBuffer.cpp
    ${SRC_DIR}/VertexBuffer.cpp
    ${SRC_DIR}/VertexArray.cpp
    ${SRC_DIR}/Debug.cpp
    ${SRC_DIR}/DebugConsole.cpp
    ${SRC_DIR}/DebugCli.cpp
    ${SRC_DIR}/tests/Test.cpp
    ${SRC_DIR}/tests/TestClearColor.cpp
    ${SRC_DIR}/vendor/glm/detail/glm.cpp
    ${SRC_DIR}/vendor/imgui/imgui.cpp
    ${SRC_DIR}/vendor/imgui/imgui_demo.cpp
    ${SRC_DIR}/vendor/imgui/imgui_draw.cpp
    ${SRC_DIR}/vendor/imgui/imgui_impl_glfw.cpp
    ${SRC_DIR}/vendor/imgui/imgui_impl_opengl3.cpp
    ${SRC_DIR}/vendor/imgui/imgui_stdlib.cpp
    ${SRC_DIR}/vendor/imgui/imgui_tables.cpp
    ${SRC_DIR}/vendor/imgui/imgui_widgets.cpp
    ${SRC_DIR}/vendor/stb_image/stb_image.cpp
)

add_executable(OpenGL ${SOURCES})

target_include_directories(OpenGL PRIVATE
    ${SRC_DIR}
    ${SRC_DIR}/vendor
)

# Three-layer debug surface: instrumentation macros, in-app console, --cli-stdin.
# OFF by default so release builds compile every macro to a no-op and the
# simulation behaves exactly as it did before any of this was added.
option(DEBUG_BUILD "Enable the debug surface (Debug.h macros, console, --cli-stdin)" OFF)
if(DEBUG_BUILD)
    target_compile_definitions(OpenGL PRIVATE DEBUG_BUILD=1)
endif()

# DebugCli reads stdin on its own thread; libstdc++ needs pthread linked explicitly.
find_package(Threads REQUIRED)
target_link_libraries(OpenGL PRIVATE Threads::Threads)

find_package(OpenGL REQUIRED)

if(WIN32)
    # Windows keeps using the libraries vendored in the repo, so this build
    # matches the MSBuild one exactly.
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(GLEW_ARCH x64)
    else()
        set(GLEW_ARCH Win32)
    endif()

    set(GLEW_STATIC_LIB "${DEP_DIR}/glew-2.1.0/lib/Release/${GLEW_ARCH}/glew32s.lib")
    set(GLFW_LIB        "${DEP_DIR}/GLFW/lib-vc2022/glfw3.lib")

    if(NOT EXISTS "${GLEW_STATIC_LIB}")
        message(FATAL_ERROR
            "Missing ${GLEW_STATIC_LIB}\n"
            "GLEW's prebuilt libs are excluded by .gitignore (the 'x64/' rule), so they are "
            "not in the repo. Download glew-2.1.0-win32.zip from "
            "https://github.com/nigels-com/glew/releases and copy its lib/ and bin/ folders "
            "into OpenGL/Dependices/glew-2.1.0/.")
    endif()

    target_include_directories(OpenGL PRIVATE
        ${DEP_DIR}/GLFW/include
        ${DEP_DIR}/glew-2.1.0/include
    )
    target_compile_definitions(OpenGL PRIVATE GLEW_STATIC)
    target_link_libraries(OpenGL PRIVATE
        ${GLEW_STATIC_LIB}
        ${GLFW_LIB}
        OpenGL::GL
        User32.lib Gdi32.lib Shell32.lib
    )
else()
    # macOS / Linux use system GLFW and GLEW (brew or apt).
    find_package(glfw3 REQUIRED)
    find_package(GLEW REQUIRED)
    target_link_libraries(OpenGL PRIVATE glfw GLEW::GLEW OpenGL::GL)

    if(APPLE)
        # Apple deprecated OpenGL in 10.14; it still works, this just silences
        # the wall of deprecation warnings.
        target_compile_definitions(OpenGL PRIVATE GL_SILENCE_DEPRECATION)

        # Produces OpenGL.app so the result is double-clickable and can be
        # packaged into a .dmg.
        set_target_properties(OpenGL PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_BUNDLE_NAME "OpenGL"
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.alexyodude.opengl"
            MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        )
    endif()
endif()

if(MSVC)
    target_compile_options(OpenGL PRIVATE /permissive-)
endif()
