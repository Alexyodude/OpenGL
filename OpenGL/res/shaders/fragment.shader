#version 430 core

out vec4 FragColor;

struct Cell {
    vec2 velocity;//2
    vec2 movement;//4
    mat4 gravity;//13
    //vec4 b;
    //vec4 a;
    float a;
    float mass;//14
    float repulse;//15
    float extra;//16
    //vec2 a;
};

layout(std430, binding = 0) buffer InputCells {
    Cell iCells[];
};

layout(std430, binding = 1) buffer OutputCells {
    Cell oCells[];
};

const int gridWidth = 1000;
const int gridHeight = 1000;

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);
    int index = coords.y * gridWidth + coords.x;

    //oMass[index] = iMass[index]; // Update the mass]

    // Map the state to color: Assuming o2 is in the range [0, 1]
    float alpha = oCells[index].mass;
    float color1 = oCells[index].gravity[1][1];
    float color3 = oCells[index].repulse;
    //float colorNorm = (color1 + color2);
    FragColor = vec4(color1*1.5, color3, alpha, alpha); // Output as grayscale
    //FragColor = vec4(iMass[index], i2[index], 0.0, 1.0);
}
