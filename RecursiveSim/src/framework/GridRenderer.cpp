#include "GridRenderer.h"
#include "../core/Log.h"

GridRenderer::GridRenderer() {
    GLCall(glGenVertexArrays(1, &m_VAO));
}

GridRenderer::~GridRenderer() {
    if (m_VAO) {
        GLCall(glDeleteVertexArrays(1, &m_VAO));
        m_VAO = 0;
    }
}

void GridRenderer::Draw() {
    GLCall(glBindVertexArray(m_VAO));
    GLCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
    GLCall(glBindVertexArray(0));
}
