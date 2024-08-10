#shader fragment
#version 330 core
out vec4 color;
in vec2 v_TexCoord;

uniform sampler2D state;
uniform vec2 u_Resolution;

void main()
{
    vec2 coords = v_TexCoord;
    color = vec4(state, state, state, 1.0);
}
