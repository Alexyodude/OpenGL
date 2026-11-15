#pragma once

#include "../framework/SimLayer.h"
#include <glm/glm.hpp>

// Two-pass particle simulation layer.
// Pass 1: velocity/state diffusion (reads SSBO[0], writes SSBO[1])
// Pass 2: mass transport (reads SSBO[1], writes SSBO[0])
struct ParticleCell {
    glm::vec2 velocity;   // xy velocity
    glm::vec2 movement;   // accumulated movement
    float mass;
    float repulse;
    float state2;
    float extra;          // Used by pass 2 (NOT padding)
};
static_assert(sizeof(ParticleCell) == 32, "ParticleCell must match GLSL std430 layout");

class ParticleLayer : public SimLayer {
public:
    ParticleLayer(int gridWidth, int gridHeight, int workgroupSize = 16)
        : SimLayer("Particles", gridWidth, gridHeight, workgroupSize) {}

    std::vector<ShaderPass> GetPasses() const override;
    void InitGrid(std::vector<uint8_t>& buffer, int parentCellIndex, const uint8_t* parentCellData) override;
    void BakeToParent(const std::vector<uint8_t>& buffer, uint8_t* parentCellData) override;
    void SetComputeUniforms(Shader& computeShader, int passIndex) override;
    int GetCellSizeBytes() const override { return sizeof(ParticleCell); }
};
