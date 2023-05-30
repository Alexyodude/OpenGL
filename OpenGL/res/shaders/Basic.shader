#shader vertex
#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in float texIndex;

out vec2 v_TexCoord;
out float v_TexIndex;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * position;
	v_TexCoord = texCoord;
	v_TexIndex = texIndex;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in float v_TexIndex;

uniform sampler2D u_Texture[3];
//uniform sampler2D u_Texture;

vec4 texColor;

void main()
{
    //texColor = texture(u_Texture[inv_TexIndex)], v_TexCoord);
    if (v_TexIndex > 0.0) {
        int Index = int(v_TexIndex);
        switch (int(Index)) {
        case 0:
            texColor = texture(u_Texture[0], v_TexCoord);
            break;
        case 1:
            texColor = texture(u_Texture[1], v_TexCoord);
            break;
        case 2:
            texColor = texture(u_Texture[2], v_TexCoord);
            break;
        // repeat for all textures
        }
    }
	//vec4 texColor = texture(u_Texture[index], v_TexCoord);
	//vec4 texColor = texture(u_Texture, v_TexCoord);
	//color = vec4(v_TexIndex, v_TexIndex, v_TexIndex, 1.0);
	color = texColor;
};