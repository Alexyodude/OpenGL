#version 430 core

layout(local_size_x = 25, local_size_y = 25) in;

// Second compute pass. Still disabled: _compute2 is commented out at the top of
// Source.cpp, so neither this shader nor its dispatch is built.
//
// It previously could not compile for two independent reasons: the final
// statement was truncated ("oCells[index].temperature" with no assignment or
// semicolon), and the Cell struct declared here disagreed with the one in
// compute.shader - it misspelled temperature as "tempature" and omitted
// "source", giving an 8-byte stride against pass 1's 12-byte layout over the
// same SSBOs. Both are fixed below so that enabling _compute2 no longer fails
// at shader compilation.
//
// The update is deliberately identical to compute.shader. What a distinct
// second pass was intended to do is not recorded anywhere, so nothing new is
// invented here.

struct Cell {
    float temperature;
    float alpha;
    float source;
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

    // Diagonals are unused by the 5-point Laplacian, same as compute.shader.
    // int upLeftIndex = up * gridWidth + left;
    // int upRightIndex = up * gridWidth + right;
    // int downLeftIndex = down * gridWidth + left;
    // int downRightIndex = down * gridWidth + right;

    Cell leftCell = iCells[leftIndex];
    Cell rightCell = iCells[rightIndex];
    Cell upCell = iCells[upIndex];
    Cell downCell = iCells[downIndex];

    oCells[index].temperature = iCells[index].temperature
    + iCells[index].alpha * (leftCell.temperature + rightCell.temperature + upCell.temperature + downCell.temperature)
    - (leftCell.alpha + rightCell.alpha + upCell.alpha + downCell.alpha) * iCells[index].temperature + iCells[index].source;
}
