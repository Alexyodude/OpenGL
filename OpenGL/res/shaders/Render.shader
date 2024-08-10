#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D u_Texture;
uniform vec2 u_Resolution;
uniform vec2 u_SlicePosition;
uniform vec2 u_SliceSize;

void main()
{
    vec2 sliceCoord = TexCoords * u_SliceSize + u_SlicePosition;
    if (sliceCoord.x < 0.0 || sliceCoord.y < 0.0 || sliceCoord.x >= u_Resolution.x || sliceCoord.y >= u_Resolution.y)
    {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Display black for out-of-bounds
    }
    else
    {
        float value = texture(u_Texture, sliceCoord / u_Resolution).r;
        FragColor = vec4(value, value, value, 1.0);
    }
}
