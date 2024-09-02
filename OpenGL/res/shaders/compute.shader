#version 430 core

layout(local_size_x = 25, local_size_y = 25) in;

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
const float divide = 0.12f;
const float divideRepulse = 0.07f;
const float leadingConst = 0.5;
const float sideConst = 0.25;
const float cleadingConst = 0.5;
const float csideConst = 0.25;

int wrapIndex(int index, int maxIndex) {
    return (index + maxIndex) % maxIndex;
}

void main() {
    ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
    int index = coords.y * gridWidth + coords.x;
    
    // Wrap the coordinates to create a torus boundary
    int left = wrapIndex(coords.x - 1, gridWidth);
    int right = wrapIndex(coords.x + 1, gridWidth);
    int up = wrapIndex(coords.y + 1, gridHeight);
    int down = wrapIndex(coords.y - 1, gridHeight);

    // Calculate wrapped indices
    int leftIndex = coords.y * gridWidth + left;
    int rightIndex = coords.y * gridWidth + right;
    int upIndex = up * gridWidth + coords.x;
    int downIndex = down * gridWidth + coords.x;

    int upLeftIndex = up * gridWidth + left;
    int upRightIndex = up * gridWidth + right;
    int downLeftIndex = down * gridWidth + left;
    int downRightIndex = down * gridWidth + right;

    // Precompute and store commonly used values in local variables
    Cell centerCell = iCells[index];
    vec2 velocity = centerCell.velocity;
    float mass = centerCell.mass;

    // Load neighboring cells only once
    Cell leftCell = iCells[leftIndex];
    Cell rightCell = iCells[rightIndex];
    Cell upCell = iCells[upIndex];
    Cell downCell = iCells[downIndex];
    Cell upLeftCell = iCells[upLeftIndex];
    Cell upRightCell = iCells[upRightIndex];
    Cell downLeftCell = iCells[downLeftIndex];
    Cell downRightCell = iCells[downRightIndex];
    
    float eff = 0.99;
    float massSplit = iCells[index].mass*0.125;

    float o00 = (downRightCell.gravity[0][0]*cleadingConst + csideConst*(downRightCell.gravity[0][1]+downRightCell.gravity[1][0]) + massSplit) * eff;
    float o01 = (downCell.gravity[0][1]*leadingConst + sideConst*(downCell.gravity[0][0]+downCell.gravity[0][2]) + massSplit) * eff;
    float o02 = (downLeftCell.gravity[0][2]*cleadingConst + csideConst*(downLeftCell.gravity[0][1]+downLeftCell.gravity[1][2]) + massSplit) * eff;
    float o10 = (rightCell.gravity[1][0]*leadingConst + sideConst*(rightCell.gravity[0][0]+rightCell.gravity[2][0]) + massSplit) * eff;
    float o12 = (leftCell.gravity[1][2]*leadingConst + sideConst*(leftCell.gravity[0][2]+leftCell.gravity[2][2]) + massSplit) * eff;
    float o20 = (upRightCell.gravity[2][0]*cleadingConst + csideConst*(upRightCell.gravity[1][0]+upRightCell.gravity[2][1]) + massSplit) * eff;
    float o21 = (upCell.gravity[2][1]*leadingConst + sideConst*(upCell.gravity[2][0]+upCell.gravity[2][2]) + massSplit) * eff;
    float o22 = (upLeftCell.gravity[2][2]*cleadingConst + csideConst*(upLeftCell.gravity[2][1]+upLeftCell.gravity[1][2]) + massSplit) * eff;

    float o11 = (o00+o01+o02+o10+o12+o20+o21+o22);
    
    vec2 stateDiff = vec2(rightCell.gravity[1][1] - leftCell.gravity[1][1], upCell.gravity[1][1] - downCell.gravity[1][1]);
    vec2 repulseDiff = vec2(rightCell.repulse - leftCell.repulse, upCell.repulse - downCell.repulse);

    vec2 diagonalStateRepulse = vec2(
        (upRightCell.gravity[1][1] - upLeftCell.gravity[1][1] + downRightCell.gravity[1][1] - downLeftCell.gravity[1][1]),
        (upLeftCell.repulse - downRightCell.repulse + upRightCell.repulse - downLeftCell.repulse)
    );

    float indexCondition = (1.0 - step(0.0, -mass));
    
    mat4 grav = mat4(o00, o01, o02, 0, o10, o11, o12, 0, o20, o21, o22, 0,0,0,0,0);

    oCells[index].gravity = grav;
    //oCells[index].gravity = ({o00, o01, o02}, {o10, o11, o12}, {o20, o21, o22});

    // Calculate new velocity
    oCells[index].velocity = (stateDiff - repulseDiff + diagonalStateRepulse + velocity) * indexCondition;

    float repulseSum = upLeftCell.repulse + upRightCell.repulse + downLeftCell.repulse + downRightCell.repulse
                     + upCell.repulse + downCell.repulse + leftCell.repulse + rightCell.repulse;

    // Update cell repulse
    oCells[index].repulse = repulseSum * divideRepulse + 1.5 * mass;

    // Copy other values directly
    oCells[index].movement = centerCell.movement;
    oCells[index].mass = iCells[index].mass;
    //oCells[index] = iCells[index];
}
