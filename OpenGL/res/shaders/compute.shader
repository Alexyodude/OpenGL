#version 430 core

layout(local_size_x = 25, local_size_y = 25) in;

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

    // int upLeftIndex = up * gridWidth + left;
    // int upRightIndex = up * gridWidth + right;
    // int downLeftIndex = down * gridWidth + left;
    // int downRightIndex = down * gridWidth + right;

    // Load neighboring cells only once
    Cell leftCell = iCells[leftIndex];
    Cell rightCell = iCells[rightIndex];
    Cell upCell = iCells[upIndex];
    Cell downCell = iCells[downIndex];
  
    // Cell upLeftCell = iCells[upLeftIndex];
    // Cell upRightCell = iCells[upRightIndex];
    // Cell downLeftCell = iCells[downLeftIndex];
    // Cell downRightCell = iCells[downRightIndex];

    oCells[index].temperature = iCells[index].temperature 
    + iCells[index].alpha * (leftCell.temperature + rightCell.temperature + upCell.temperature + downCell.temperature)
    - (leftCell.alpha + rightCell.alpha + upCell.alpha + downCell.alpha) * iCells[index].temperature + iCells[index].source;

    // Compute differences between neighboring states and repulses
    // vec2 stateDiff = vec2(rightCell.state2 - leftCell.state2, upCell.state2 - downCell.state2);
    // vec2 repulseDiff = vec2(rightCell.repulse - leftCell.repulse, upCell.repulse - downCell.repulse);

    // vec2 diagonalStateRepulse = vec2(
    //     (upRightCell.state2 - upLeftCell.state2 + downRightCell.state2 - downLeftCell.state2) * 0.5,
    //     (upLeftCell.repulse - downRightCell.repulse + upRightCell.repulse - downLeftCell.repulse) * 0.5
    // );

    // float indexCondition = (1.0 - step(0.0, -mass));
}
