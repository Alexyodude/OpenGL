#version 430 core

out vec4 FragColor;

struct ChemicalsCell {
    float temperature;
    float density;
    float bondStrength;
    float reactivity;
};

layout(std430, binding = 0) buffer Cells {
    ChemicalsCell cells[];
};

uniform int u_GridWidth;
uniform int u_GridHeight;
uniform int u_ViewportWidth;
uniform int u_ViewportHeight;

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(float(u_ViewportWidth), float(u_ViewportHeight));
    int cellX = clamp(int(uv.x * float(u_GridWidth)),  0, u_GridWidth  - 1);
    int cellY = clamp(int(uv.y * float(u_GridHeight)), 0, u_GridHeight - 1);
    int index = cellY * u_GridWidth + cellX;

    ChemicalsCell c = cells[index];

    // Map 4 fields to RGBA for a distinctive chemical display
    float r = clamp(c.temperature,  0.0, 1.0);
    float g = clamp(c.density,      0.0, 1.0);
    float b = clamp(c.bondStrength, 0.0, 1.0);
    float a = 1.0;

    FragColor = vec4(r, g, b, a);
}
