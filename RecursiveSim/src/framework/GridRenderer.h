#pragma once

#include <GL/glew.h>

// Renders a fullscreen quad using GL_TRIANGLE_STRIP with 4 vertices.
// No VBO needed -- vertex positions are generated in the vertex shader
// from gl_VertexID (matches Heat branch pattern).
class GridRenderer {
public:
    GridRenderer();
    ~GridRenderer();

    void Draw();

private:
    GLuint m_VAO = 0;
};
