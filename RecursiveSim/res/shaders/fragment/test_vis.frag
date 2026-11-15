#version 430 core

out vec4 FragColor;

struct TestCell {
    float alive;
    float age;
    float padding0;
    float padding1;
};

layout(std430, binding = 0) buffer Cells {
    TestCell cells[];
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

    float alive = cells[index].alive;
    float age   = clamp(cells[index].age / 50.0, 0.0, 1.0);

    // Dead = black; young alive = bright green; old alive = white
    float r = alive * age;
    float g = alive * (0.5 + 0.5 * (1.0 - age));
    float b = alive * age * 0.5;

    FragColor = vec4(r, g, b, 1.0);
}
