#pragma once

#include "../framework/SimLayer.h"
#include <glm/glm.hpp>

// Single-pass spring-mass bond simulation layer.
// Cells have: displacement, velocity, mass, stiffness, damping, broken flag.
struct BondCell {
    glm::vec2 displacement;
    glm::vec2 velocity;
    float mass;
    float stiffness;
    float damping;
    float broken;   // 0.0 = intact, 1.0 = broken
};
static_assert(sizeof(BondCell) == 32, "BondCell must match GLSL std430 layout");

class BondLayer : public SimLayer {
public:
    BondLayer(int gridWidth, int gridHeight);

    std::vector<ShaderPass> GetPasses() const override;
    void InitGrid(std::vector<uint8_t>& buffer, int parentCellIndex, const uint8_t* parentCellData) override;
    void BakeToParent(const std::vector<uint8_t>& buffer, uint8_t* parentCellData) override;
    void SetComputeUniforms(Shader& computeShader, int passIndex) override;
    int GetCellSizeBytes() const override { return sizeof(BondCell); }
};
