#version 330 core
out float FragColor;
in vec2 TexCoords;

uniform sampler2D u_Texture;
uniform vec2 u_Resolution;

float getCellvalue(int x, int y) {
    return texture(u_Texture, (vec2(x, y) + 0.5) / u_Resolution).r;
}

void main() {
    ivec2 coords = ivec2(TexCoords * u_Resolution);

    int aliveNeighbors = int(getCellvalue(coords.x - 1, coords.y - 1)) +
						int(getCellvalue(coords.x, coords.y - 1)) +
						int(getCellvalue(coords.x + 1, coords.y - 1)) +
						int(getCellvalue(coords.x - 1, coords.y)) +
						int(getCellvalue(coords.x + 1, coords.y)) +
						int(getCellvalue(coords.x - 1, coords.y + 1)) +
						int(getCellvalue(coords.x, coords.y + 1)) +
						int(getCellvalue(coords.x + 1, coords.y + 1));

    float currentState = getCellvalue(coords.x, coords.y);
    if (currentState == 1.0 && (aliveNeighbors < 2 || aliveNeighbors > 3)) {
        FragColor = 0.0;
    } else if (currentState == 0.0 && aliveNeighbors == 3) {
        FragColor = 1.0;
    } else {
        FragColor = currentState;
    }
}
