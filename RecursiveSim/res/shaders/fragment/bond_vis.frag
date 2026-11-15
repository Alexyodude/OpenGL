#version 430 core

out vec4 FragColor;

struct BondCell {
    vec2 displacement;
    vec2 velocity;
    float mass;
    float stiffness;
    float damping;
    float broken;
};

layout(std430, binding = 0) buffer Cells {
    BondCell cells[];
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

    BondCell b = cells[index];

    float dispMag  = clamp(length(b.displacement) * 2.0, 0.0, 1.0);
    float broken   = clamp(b.broken, 0.0, 1.0);
    float stiffness = clamp(b.stiffness, 0.0, 1.0);

    // red = broken, green = stiff, blue = 0, alpha = displacement magnitude
    float r  = broken;
    float g  = (1.0 - broken) * stiffness;
    float bl = 0.0;
    float a  = 0.2 + 0.8 * dispMag;  // min alpha so grid is always visible

    FragColor = vec4(r, g, bl, a);
}
