#version 430 core

out vec4 FragColor;

layout(std430, binding = 0) buffer InputCells {
    float iMass[]; // Array of floats
    float i2[];          // Array of ints
};

layout(std430, binding = 1) buffer OutputCells {
    float oMass[]; // Array of floats
    float o2[];            // Array of ints
};

int gridWidth = 1000;
int gridHeight = 1000;
const float divide = 0.01f;

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);

    int index = coords.y * gridWidth + coords.x;

    // Perform the operations
    oMass[index] = iMass[index];

    //o2[index] = (i2[index + 1] + i2[index - 1] + i2[index + gridWidth] + i2[index - gridWidth]) * divide + iMass[index];
    o2[index] = i2[index-1];

    // Use output to generate a color
    FragColor = vec4(0, 0, o2[index], 1); // Assuming o2 is in the range [0, 255]
}
