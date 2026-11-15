#pragma once

#include <GL/glew.h>
#include <vector>
#include <cstdint>
#include <cstddef>

// Manages two ping-pong SSBOs for compute shader dispatch.
// Explicit read/write index control -- callers pass exact SSBO indices,
// no implicit current/other assumptions.
class SSBOManager {
public:
    SSBOManager() = default;
    ~SSBOManager();

    // Allocate two SSBOs of cellCount * cellSize bytes each.
    void Allocate(size_t cellCount, size_t cellSize);

    // Upload data to SSBO[0]. Call before first compute dispatch.
    void Upload(const std::vector<uint8_t>& data);

    // Upload data to SSBO[m_CurrentIndex]. Use after Download() to write back edits.
    void UploadToCurrent(const std::vector<uint8_t>& data);

    // Download data from SSBO[m_CurrentIndex].
    // Calls glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT) before readback.
    void Download(std::vector<uint8_t>& data);

    // Bind SSBO[readIdx] to binding 0 and SSBO[writeIdx] to binding 1.
    // Takes explicit indices -- pass 0 or 1 directly.
    void BindForCompute(int readIdx, int writeIdx);

    // Bind both SSBOs for fragment shader read (binding 0 = current, binding 1 = other).
    void BindForRender();

    // Swap the current index (0 <-> 1).
    void SwapBuffers();

    // Free GPU buffers.
    void Deallocate();

    int GetCurrentIndex() const { return m_CurrentIndex; }
    size_t GetBufferSize() const { return m_BufferSize; }
    bool IsAllocated() const { return m_SSBOs[0] != 0; }

private:
    GLuint m_SSBOs[2] = { 0, 0 };
    int m_CurrentIndex = 0;
    size_t m_BufferSize = 0;
};
