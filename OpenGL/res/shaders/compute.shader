#version 430 core

layout(local_size_x = 25, local_size_y = 25) in;

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
    Cell iCells[]; // Array of input cells
};

layout(std430, binding = 1) buffer OutputCells {
    Cell oCells[]; // Array of output cells
};

const int gridWidth = 1000;
const int gridHeight = 1000;
const float divide = 0.11f;
const float divideRepulse = 0.08f;

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

    float indexCondition = (1-step(0, -iCells[index].mass));

    oCells[index].velocity = (vec2(
        iCells[rightIndex].state2 - iCells[leftIndex].state2 
        - iCells[rightIndex].repulse + iCells[leftIndex].repulse
        + (iCells[upRightIndex].state2 + iCells[downRightIndex].state2 - iCells[upLeftIndex].state2 - iCells[downLeftIndex].state2
        - iCells[upRightIndex].repulse - iCells[downRightIndex].repulse + iCells[upLeftIndex].repulse + iCells[downLeftIndex].repulse) * 0.5
        , 
        iCells[upIndex].state2 - iCells[downIndex].state2
        - iCells[upIndex].repulse + iCells[downIndex].repulse
        + (iCells[upLeftIndex].state2 + iCells[upRightIndex].state2 - iCells[downLeftIndex].state2 - iCells[downRightIndex].state2
        - iCells[upLeftIndex].repulse - iCells[upRightIndex].repulse + iCells[downLeftIndex].repulse + iCells[downRightIndex].repulse) * 0.5)
        
        + iCells[index].velocity*0.99) * indexCondition;

    // Perform the operations using the wrapped indices
    oCells[index].state2 = (iCells[upLeftIndex].state2 + iCells[upRightIndex].state2 + iCells[downLeftIndex].state2 + iCells[downRightIndex].state2 +
        iCells[upIndex].state2 + iCells[downIndex].state2 + iCells[leftIndex].state2 + iCells[rightIndex].state2)*divide + iCells[index].mass;
    oCells[index].repulse = (iCells[upLeftIndex].repulse + iCells[upRightIndex].repulse + iCells[downLeftIndex].repulse + iCells[downRightIndex].repulse +
        iCells[upIndex].repulse + iCells[downIndex].repulse + iCells[leftIndex].repulse + iCells[rightIndex].repulse)*divideRepulse + 1.3 * iCells[index].mass;

    oCells[index].movement = iCells[index].movement;
    oCells[index].mass = iCells[index].mass;
    //oCells[index].mass = iCells[index].mass;
}