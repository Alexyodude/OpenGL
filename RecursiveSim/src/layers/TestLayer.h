#pragma once

#include "../framework/SimLayer.h"
#include <cstdint>

// Extensibility test layer: Conway's Game of Life rules.
// Proves that adding a 5th layer requires ONLY:
//   (a) this new class + compute shader + fragment shader
//   (b) one registration line in LayerRegistry
// Zero framework changes needed.
struct TestCell {
    float alive;    // 1.0 = alive, 0.0 = dead
    float age;      // how many steps this cell has been alive
    float padding0; // std430 alignment
    float padding1;
};
static_assert(sizeof(TestCell) == 16, "TestCell must match GLSL std430 layout");

class TestLayer : public SimLayer {
public:
    TestLayer(int gridWidth, int gridHeight, int workgroupSize = 16)
        : SimLayer("GameOfLife", gridWidth, gridHeight, workgroupSize) {}

    std::vector<ShaderPass> GetPasses() const override;
    void InitGrid(std::vector<uint8_t>& buffer, int parentCellIndex, const uint8_t* parentCellData) override;
    void BakeToParent(const std::vector<uint8_t>& buffer, uint8_t* parentCellData) override;
    void SetComputeUniforms(Shader& computeShader, int passIndex) override;
    int GetCellSizeBytes() const override { return sizeof(TestCell); }
};
