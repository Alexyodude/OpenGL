// ShaderSource.h
#pragma once

namespace ShaderSource {
    const char* BasicVertexShader = R"(
        #version 330 core

        layout (location = 0) in vec4 position;
        layout (location = 1) in vec4 color;

        uniform mat4 u_MVP;

        out vec4 v_Color;

        void main()
        {
            v_Color = color;
            gl_Position = u_MVP * position;
        };
    )";

    const char* BasicFragmentShader = R"(
        #version 330 core

        layout(location = 0) out vec4 color;

        in vec4 v_Color;

        void main()
        {
            color = v_Color;
        }
    )";
}
