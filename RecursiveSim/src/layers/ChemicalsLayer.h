#pragma once

#include "../framework/SimLayer.h"
#include <glm/glm.hpp>

// Top-level static display layer. No compute shader.
// Cells have: temperature, density, bondStrength, reactivity.
struct ChemicalsCell {
    float temperature;
    float density;
    float bondStrength;
    float reactivity;
};
static_assert(sizeof(ChemicalsCell) == 16, "ChemicalsCell must match GLSL std430 layout");

class ChemicalsLayer : public SimLayer {
public:
    ChemicalsLayer(int gridWidth, int gridHeight);

    std::vector<ShaderPass> GetPasses() const override;
    void InitGrid(std::vector<uint8_t>& buffer, int parentCellIndex, const uint8_t* parentCellData) override;
    void BakeToParent(const std::vector<uint8_t>& buffer, uint8_t* parentCellData) override;
    void SetComputeUniforms(Shader& computeShader, int passIndex) override;
    int GetCellSizeBytes() const override { return sizeof(ChemicalsCell); }
};
