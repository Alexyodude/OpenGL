#version 430 core

out vec4 FragColor;

struct Cell {
    vec2 velocity;
    vec2 movement;
    float mass;
    float repulse;
    float state2;
    float test1;
    //float test2;
    //float test3;
    //float test4;
    //vec2 test;
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
    ivec2 coords = ivec2(gl_FragCoord.xy/4);
    int index = coords.y * gridWidth + coords.x;

    //oMass[index] = iMass[index]; // Update the mass]

    // Map the state to color: Assuming o2 is in the range [0, 1]
    float alpha = oCells[index].mass;
    float color2 = oCells[index].state2;
    float color3 = oCells[index].repulse;
    //float colorNorm = (color1 + color2);
    FragColor = vec4(log(color2)*0.5, 0, log(color3)*0.5, alpha); // Output as grayscale
    //FragColor = vec4(iMass[index], i2[index], 0.0, 1.0);
}
