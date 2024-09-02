#version 430 core

layout(local_size_x = 25, local_size_y = 25) in;

struct Cell {
    vec2 velocity;
    vec2 movement;
    mat4 gravity;
    float a;
    float mass;
    float repulse;
    float extra;
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

    int xLoc = int(sign(iCells[index].movement.x) * step(1, iCells[index].movement.x * iCells[index].movement.x));
    int yLoc = int(sign(iCells[index].movement.y) * step(1, iCells[index].movement.y * iCells[index].movement.y));
    int movementTargetIndex = wrapIndex(coords.x + xLoc, gridWidth) + wrapIndex(coords.y + yLoc, gridHeight) * gridHeight;

    // Wrap the coordinates to create a torus boundary
    int left = wrapIndex(coords.x - 1, gridWidth);
    int right = wrapIndex(coords.x + 1, gridWidth);
    int up = wrapIndex(coords.y + 1, gridHeight);
    int down = wrapIndex(coords.y - 1, gridHeight);

    int leftIndex = coords.y * gridWidth + left;
    int rightIndex = coords.y * gridWidth + right;
    int upIndex = up * gridWidth + coords.x;
    int downIndex = down * gridWidth + coords.x;

    int upLeftIndex = up * gridWidth + left;
    int upRightIndex = up * gridWidth + right;
    int downLeftIndex = down * gridWidth + left;
    int downRightIndex = down * gridWidth + right;

    float upLeftCondition = step(1, iCells[upLeftIndex].movement.x) * step(1, -iCells[upLeftIndex].movement.y);
    float upCondition = (1 - step(1, iCells[upIndex].movement.x * iCells[upIndex].movement.x)) * step(1, -iCells[upIndex].movement.y);
    float upRightCondition = step(1, -iCells[upRightIndex].movement.x) * step(1, -iCells[upRightIndex].movement.y);
    float leftCondition = step(1, iCells[leftIndex].movement.x) * (1 - step(1, iCells[leftIndex].movement.y * iCells[leftIndex].movement.y));
    float rightCondition = step(1, -iCells[rightIndex].movement.x) * (1 - step(1, iCells[rightIndex].movement.y * iCells[rightIndex].movement.y));
    float downLeftCondition = step(1, iCells[downLeftIndex].movement.x) * step(1, iCells[downLeftIndex].movement.y);
    float downCondition = (1 - step(1, iCells[downIndex].movement.x * iCells[downIndex].movement.x)) * step(1, iCells[downIndex].movement.y);
    float downRightCondition = step(1, -iCells[downRightIndex].movement.x) * step(1, iCells[downRightIndex].movement.y);

    float indexCondition = (1 - step(1, iCells[index].movement.x * iCells[index].movement.x)) * (1 - step(1, iCells[index].movement.y * iCells[index].movement.y));

    // Number of surrounding cells moving into the index cell
    float numberOfMovementIntoIndexCondition = upLeftCondition + upCondition + upRightCondition +
                                                        leftCondition + indexCondition + rightCondition +
                                                        downLeftCondition + downCondition + downRightCondition;

    // Check if there is movement into the index from the surrounding cells
    float checkMovementOfSurrounding = upLeftCondition + upCondition + upRightCondition +
                                                leftCondition + rightCondition +
                                                downLeftCondition + downCondition + downRightCondition;

    if (iCells[index].mass > 0) {
        if (numberOfMovementIntoIndexCondition == 1 && indexCondition == 0) {
            // Move mass and velocity if only one cell is moving into the index cell
            float mass = iCells[upLeftIndex].mass * upLeftCondition +
                         iCells[upIndex].mass * upCondition +
                         iCells[upRightIndex].mass * upRightCondition +
                         iCells[leftIndex].mass * leftCondition +
                         iCells[rightIndex].mass * rightCondition +
                         iCells[downLeftIndex].mass * downLeftCondition +
                         iCells[downIndex].mass * downCondition +
                         iCells[downRightIndex].mass * downRightCondition;

            //float massNull = step(0, -mass) * 0.0001;

            vec2 velocity = (iCells[upLeftIndex].velocity * iCells[upLeftIndex].mass * upLeftCondition +
                             iCells[upIndex].velocity * iCells[upIndex].mass * upCondition +
                             iCells[upRightIndex].velocity * iCells[upRightIndex].mass * upRightCondition +
                             iCells[leftIndex].velocity * iCells[leftIndex].mass * leftCondition +
                             iCells[rightIndex].velocity * iCells[rightIndex].mass * rightCondition +
                             iCells[downLeftIndex].velocity * iCells[downLeftIndex].mass * downLeftCondition +
                             iCells[downIndex].velocity * iCells[downIndex].mass * downCondition +
                             iCells[downRightIndex].velocity * iCells[downRightIndex].mass * downRightCondition) / (mass);

            oCells[index].movement = ((iCells[upLeftIndex].movement + vec2(-1, 1)) * iCells[upLeftIndex].mass * upLeftCondition +
                                      (iCells[upIndex].movement + vec2(0, 1)) * iCells[upIndex].mass * upCondition +
                                      (iCells[upRightIndex].movement + vec2(1, 1)) * iCells[upRightIndex].mass * upRightCondition +
                                      (iCells[leftIndex].movement + vec2(-1, 0)) * iCells[leftIndex].mass * leftCondition +
                                      (iCells[rightIndex].movement + vec2(1, 0)) * iCells[rightIndex].mass * rightCondition +
                                      (iCells[downLeftIndex].movement + vec2(-1, -1)) * iCells[downLeftIndex].mass * downLeftCondition +
                                      (iCells[downIndex].movement + vec2(0, -1)) * iCells[downIndex].mass * downCondition +
                                      (iCells[downRightIndex].movement + vec2(1, -1)) * iCells[downRightIndex].mass * downRightCondition) / (mass) + velocity * 0.01;

            oCells[index].velocity = velocity;
            oCells[index].mass = mass;
            oCells[index].repulse = iCells[index].repulse;
            oCells[index].gravity = iCells[index].gravity;
        }else if(numberOfMovementIntoIndexCondition == 0 && indexCondition == 0 ){
            oCells[index].movement = vec2(0);
            oCells[index].velocity = vec2(0);
            oCells[index].mass = 0;
            oCells[index].repulse = iCells[index].repulse;
            oCells[index].gravity = iCells[index].gravity;

        }else {
            // Keep the cell's properties unchanged if no movement into the index cell
            oCells[index].movement = iCells[index].movement + iCells[index].velocity*0.01;
            oCells[index].velocity = iCells[index].velocity;
            oCells[index].mass = iCells[index].mass;
            oCells[index].repulse = iCells[index].repulse;
            oCells[index].gravity = iCells[index].gravity;
        }
    } else {
            if (checkMovementOfSurrounding > 0) {
            // Move cell to the index cell if there is movement into the index from surrounding cells
            vec2 velocity = (iCells[upLeftIndex].velocity * upLeftCondition +
                             iCells[upIndex].velocity * upCondition +
                             iCells[upRightIndex].velocity * upRightCondition +
                             iCells[leftIndex].velocity * leftCondition +
                             iCells[rightIndex].velocity * rightCondition +
                             iCells[downLeftIndex].velocity * downLeftCondition +
                             iCells[downIndex].velocity * downCondition +
                             iCells[downRightIndex].velocity * downRightCondition);

            float mass = (iCells[upLeftIndex].mass * upLeftCondition +
                          iCells[upIndex].mass * upCondition +
                          iCells[upRightIndex].mass * upRightCondition +
                          iCells[leftIndex].mass * leftCondition +
                          iCells[rightIndex].mass * rightCondition +
                          iCells[downLeftIndex].mass * downLeftCondition +
                          iCells[downIndex].mass * downCondition +
                          iCells[downRightIndex].mass * downRightCondition);

            oCells[index].movement = ((iCells[upLeftIndex].movement + vec2(-1, 1)) * upLeftCondition +
                                      (iCells[upIndex].movement + vec2(0, 1)) * upCondition +
                                      (iCells[upRightIndex].movement + vec2(1, 1)) * upRightCondition +
                                      (iCells[leftIndex].movement + vec2(-1, 0)) * leftCondition +
                                      (iCells[rightIndex].movement + vec2(1, 0)) * rightCondition +
                                      (iCells[downLeftIndex].movement + vec2(-1, -1)) * downLeftCondition +
                                      (iCells[downIndex].movement + vec2(0, -1)) * downCondition +
                                      (iCells[downRightIndex].movement + vec2(1, -1)) * downRightCondition) + velocity * 0.01;

            oCells[index].velocity = velocity;
            oCells[index].mass = mass;
            oCells[index].repulse = iCells[index].repulse;
            oCells[index].gravity = iCells[index].gravity;
            }else{
        // No mass, keep properties unchanged
        oCells[index].movement = iCells[index].movement;
        oCells[index].velocity = iCells[index].velocity + iCells[index].velocity*0.01;
        oCells[index].mass = iCells[index].mass;
        oCells[index].repulse = iCells[index].repulse;
        oCells[index].gravity = iCells[index].gravity;
        }
    }
}
