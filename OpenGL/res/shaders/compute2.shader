#version 430 core

layout(local_size_x = 25, local_size_y = 25) in;

struct Cell {
    float tempature;
    float alpha;
};

layout(std430, binding = 0) buffer InputCells {
    Cell iCells[]; // Array of input cells
};

layout(std430, binding = 1) buffer OutputCells {
    Cell oCells[]; // Array of output cells
};

const int gridWidth = 1000;
const int gridHeight = 1000;

int wrapIndex(int index, int maxIndex) {
    return (index + maxIndex) % maxIndex;
}

void main() {

    ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
    int index = coords.y * gridWidth + coords.x;

    // Wrap the coordinates to create a torus boundary
    int left = wrapIndex(coords.x - 1, gridWidth);
    int right = wrapIndex(coords.x + 1, gridWidth);
    int up = wrapIndex(coords.y - 1, gridHeight);
    int down = wrapIndex(coords.y + 1, gridHeight);

    // Calculate wrapped indices
    int leftIndex = coords.y * gridWidth + left;
    int rightIndex = coords.y * gridWidth + right;
    int upIndex = up * gridWidth + coords.x;
    int downIndex = down * gridWidth + coords.x;

    int upLeftIndex = up * gridWidth + left;
    int upRightIndex = up * gridWidth + right;
    int downLeftIndex = down * gridWidth + left;
    int downRightIndex = down * gridWidth + right;

    oCells[index].temperature
}