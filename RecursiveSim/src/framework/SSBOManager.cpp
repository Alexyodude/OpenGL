#include "SSBOManager.h"
#include "../core/Log.h"
#include <cassert>
#include <iostream>

SSBOManager::~SSBOManager() {
    Deallocate();
}

void SSBOManager::Allocate(size_t cellCount, size_t cellSize) {
    assert(!IsAllocated() && "SSBOManager already allocated -- call Deallocate() first");
    m_BufferSize = cellCount * cellSize;
    m_CurrentIndex = 0;

    GLCall(glGenBuffers(2, m_SSBOs));

    for (int i = 0; i < 2; i++) {
        GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBOs[i]));
        GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizeiptr)m_BufferSize, nullptr, GL_DYNAMIC_COPY));
    }
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

void SSBOManager::Upload(const std::vector<uint8_t>& data) {
    assert(IsAllocated());
    assert(data.size() <= m_BufferSize);
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBOs[0]));
    GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (GLsizeiptr)data.size(), data.data()));
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

void SSBOManager::UploadToCurrent(const std::vector<uint8_t>& data) {
    assert(IsAllocated());
    assert(data.size() <= m_BufferSize);
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBOs[m_CurrentIndex]));
    GLCall(glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (GLsizeiptr)data.size(), data.data()));
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

void SSBOManager::Download(std::vector<uint8_t>& data) {
    assert(IsAllocated());
    // Required before CPU readback to ensure GPU writes are visible
    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

    data.resize(m_BufferSize);
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBOs[m_CurrentIndex]));
    GLCall(glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (GLsizeiptr)m_BufferSize, data.data()));
    GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

void SSBOManager::BindForCompute(int readIdx, int writeIdx) {
    assert(readIdx == 0 || readIdx == 1);
    assert(writeIdx == 0 || writeIdx == 1);
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBOs[readIdx]));
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_SSBOs[writeIdx]));
}

void SSBOManager::BindForRender() {
    assert(IsAllocated());
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBOs[m_CurrentIndex]));
    GLCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_SSBOs[(m_CurrentIndex + 1) % 2]));
}

void SSBOManager::SwapBuffers() {
    m_CurrentIndex = (m_CurrentIndex + 1) % 2;
}

void SSBOManager::Deallocate() {
    if (IsAllocated()) {
        GLCall(glDeleteBuffers(2, m_SSBOs));
        m_SSBOs[0] = 0;
        m_SSBOs[1] = 0;
        m_CurrentIndex = 0;
        m_BufferSize = 0;
    }
}
