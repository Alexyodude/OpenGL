#version 430 core

out vec4 FragColor;

struct HeatCell {
    float temperature;
    float alpha;
    float source;
};

layout(std430, binding = 0) buffer Cells {
    HeatCell cells[];
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

    float temp = cells[index].temperature;

    // Map temperature to color: cold=blue, warm=green, hot=red
    // Normalize: assume temperature range roughly 0..3
    float t = clamp(temp / 3.0, 0.0, 1.0);

    float r = clamp(2.0 * t - 1.0, 0.0, 1.0);
    float g = clamp(1.0 - abs(2.0 * t - 1.0), 0.0, 1.0);
    float b = clamp(1.0 - 2.0 * t, 0.0, 1.0);

    FragColor = vec4(r, g, b, 1.0);
}
