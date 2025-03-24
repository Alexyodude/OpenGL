#version 430 core

out vec4 FragColor;

struct Cell {
    float temperature;
    float alpha;
    float source;
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
    float r = oCells[index].temperature/100;
    float g = oCells[index].temperature/200;
    float b = oCells[index].temperature/300;
    float a = oCells[index].temperature/300;
    //float colorNorm = (color1 + color2);
    FragColor = vec4(r, g, b, a); // Output as grayscale
    // FragColor = vec4(coords.x/800.0, coords.y/800.0, 0.0, 1.0); Output as grayscale)
    //FragColor = vec4(iMass[index], i2[index], 0.0, 1.0);
}
