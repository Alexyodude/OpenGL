#version 430 core

out vec4 FragColor;

struct ParticleCell {
    vec2 velocity;
    vec2 movement;
    float mass;
    float repulse;
    float state2;
    float extra;
};

layout(std430, binding = 0) buffer Cells {
    ParticleCell cells[];
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

    ParticleCell p = cells[index];

    float speed = length(p.velocity);
    float mass  = clamp(p.mass, 0.0, 1.0);

    // Speed -> green channel, mass -> red channel
    float r = mass;
    float g = clamp(speed * 2.0, 0.0, 1.0);
    float b = clamp(p.repulse, 0.0, 1.0);

    FragColor = vec4(r, g, b, 1.0);
}
