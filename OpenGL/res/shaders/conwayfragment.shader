#version 430 core

out vec4 FragColor; // Use vec4 for color output (RGBA)

layout(binding = 0) buffer InputCells {
    int inputCells[];
};

layout(binding = 1) buffer OutputCells {
    int outputCells[];
};

int gridWidth = 1000; // Set the width of the grid
int gridHeight = 1000; // Set the height of the grid

int getCellvalue(int x, int y) {
    if (x < 0 || x >= gridWidth || y < 0 || y >= gridHeight) {
        return 0;
    }
    return inputCells[y * gridWidth + x];
}

void main() {
    // Use gl_FragCoord directly as grid coordinates
    ivec2 coords = ivec2(gl_FragCoord.xy);

    // Check if the coordinates are within the grid
    if (coords.x < 0 || coords.x >= gridWidth || coords.y < 0 || coords.y >= gridHeight) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Outside the grid, render as black
        return;
    }

    int aliveNeighbors = getCellvalue(coords.x - 1, coords.y - 1) +
                         getCellvalue(coords.x, coords.y - 1) +
                         getCellvalue(coords.x + 1, coords.y - 1) +
                         getCellvalue(coords.x - 1, coords.y) +
                         getCellvalue(coords.x + 1, coords.y) +
                         getCellvalue(coords.x - 1, coords.y + 1) +
                         getCellvalue(coords.x, coords.y + 1) +
                         getCellvalue(coords.x + 1, coords.y + 1);

    int currentState = getCellvalue(coords.x, coords.y);
    if (currentState == 1 && (aliveNeighbors < 2 || aliveNeighbors > 3)) {
        outputCells[coords.y * gridWidth + coords.x] = 0;
    } else if (currentState == 0 && aliveNeighbors == 3) {
        outputCells[coords.y * gridWidth + coords.x] = 1;
    } else {
        outputCells[coords.y * gridWidth + coords.x] = currentState;
    }

    // Map the state to color: 1.0 for alive, 0.0 for dead
    float color = float(outputCells[coords.y * gridWidth + coords.x]);
    FragColor = vec4(color, color, color, 1.0); // Output as grayscale
}
