#version 430 core

layout(local_size_x = 25, local_size_y = 25) in;

struct Cell {
    vec2 velocity;
    vec2 movement;
    float mass;
    float repulse;
    float state2;
    float test1;
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
const float divideRepulse = 0.07f;

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

    float upLeftCondition = step(1, iCells[upLeftIndex].movement.x)*step(1, -iCells[upLeftIndex].movement.y);
    float upCondition = (1-step(1, iCells[upIndex].movement.x*iCells[upIndex].movement.x))*step(1, -iCells[upIndex].movement.y);
    float upRightCondition = step(1, -iCells[upRightIndex].movement.x)*step(1, -iCells[upRightIndex].movement.y);
    float leftCondition = step(1, iCells[leftIndex].movement.x)*(1-step(1, iCells[leftIndex].movement.y*iCells[leftIndex].movement.y));
    float rightCondition = step(1, -iCells[rightIndex].movement.x)*(1-step(1, iCells[rightIndex].movement.y*iCells[rightIndex].movement.y));
    float downLeftCondition = step(1, iCells[downLeftIndex].movement.x)*step(1, iCells[downLeftIndex].movement.y);
    float downCondition = (1-step(1, iCells[downIndex].movement.x*iCells[downIndex].movement.x))*step(1, iCells[downIndex].movement.y);
    float downRightCondition = step(1, -iCells[downRightIndex].movement.x)*step(1, iCells[downRightIndex].movement.y);

    //moving then 0 if not then 1
    float indexCondition = (1-step(1, iCells[index].movement.x*iCells[index].movement.x))*(1-step(1, iCells[index].movement.y*iCells[index].movement.y));
    
    float mass = iCells[upLeftIndex].mass*upLeftCondition
             + iCells[upIndex].mass*upCondition
             + iCells[upRightIndex].mass*upRightCondition
             + iCells[leftIndex].mass*leftCondition
             + iCells[index].mass*indexCondition
             + iCells[rightIndex].mass*rightCondition
		     + iCells[downLeftIndex].mass*downLeftCondition
             + iCells[downIndex].mass*downCondition
             + iCells[downRightIndex].mass*downRightCondition;

    float massState = step(0,-mass);
    float massNull = massState*0.0001;

    // Calculate the new state of the cell
    
    vec2 velocity = (          iCells[upLeftIndex].velocity*iCells[upLeftIndex].mass * upLeftCondition
                             + iCells[upIndex].velocity*iCells[upIndex].mass * upCondition
                             + iCells[upRightIndex].velocity*iCells[upRightIndex].mass * upRightCondition
                             + iCells[leftIndex].velocity*iCells[leftIndex].mass * leftCondition
                             + iCells[index].velocity*iCells[index].mass * indexCondition
                             + iCells[rightIndex].velocity*iCells[rightIndex].mass * rightCondition
                             + iCells[downLeftIndex].velocity*iCells[downLeftIndex].mass * downLeftCondition
                             + iCells[downIndex].velocity*iCells[downIndex].mass  * downCondition
                             + iCells[downRightIndex].velocity*iCells[downRightIndex].mass * downRightCondition
                             )/(mass+massNull);
    oCells[index].movement = ( (iCells[upLeftIndex].movement+vec2(-1,1))*iCells[upLeftIndex].mass * upLeftCondition
                             + (iCells[upIndex].movement+vec2(0,1))*iCells[upIndex].mass * upCondition
                             + (iCells[upRightIndex].movement+vec2(1,1))*iCells[upRightIndex].mass * upRightCondition
                             + (iCells[leftIndex].movement+vec2(-1,0))*iCells[leftIndex].mass * leftCondition
                             + iCells[index].movement*iCells[index].mass * indexCondition
                             + (iCells[rightIndex].movement+vec2(1,0))*iCells[rightIndex].mass * rightCondition
                             + (iCells[downLeftIndex].movement+vec2(-1,-1))*iCells[downLeftIndex].mass * downLeftCondition
                             + (iCells[downIndex].movement+vec2(0,-1))*iCells[downIndex].mass * downCondition
                             + (iCells[downRightIndex].movement+vec2(1, -1))*iCells[downRightIndex].mass * downRightCondition
                             )/(mass+massNull)+velocity*0.01;

    oCells[index].velocity = velocity;
    oCells[index].mass = mass;

    oCells[index].repulse = (iCells[index].repulse-2.0*iCells[index].mass)*(1-indexCondition)+(iCells[index].repulse+2*(mass-iCells[index].mass))*indexCondition;
    oCells[index].state2 = (iCells[index].state2-iCells[index].mass)*(1-indexCondition)+(iCells[index].state2+mass-iCells[index].mass)*indexCondition;
    //oCells[index].mass = iCells[index].mass + 0.5;
}