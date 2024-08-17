#version 430 core

layout(local_size_x = 16, local_size_y = 16) in;

layout(std430, binding = 0) buffer InputCells {
    float iMass[]; // Array of floats
    float i2[];    // Array of floats
};

layout(std430, binding = 1) buffer OutputCells {
    float oMass[]; // Array of floats
    float o2[];    // Array of floats
};

uniform int gridWidth;
uniform int gridHeight;
const float divide = 0.01f;

void main() {
    ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
    int index = coords.y * gridWidth + coords.x;

    if (coords.x < 1 || coords.x >= gridWidth - 1 || coords.y < 1 || coords.y >= gridHeight - 1) {
        // Handle boundaries (simple case: copy the mass)
        oMass[index] = iMass[index];
        o2[index] = i2[index];
        return;
    }

    // Perform the operations
    oMass[index] = iMass[index];
    o2[index] = (i2[index + 1] + i2[index - 1] + i2[index + gridWidth] + i2[index - gridWidth]) * divide + iMass[index];
}
