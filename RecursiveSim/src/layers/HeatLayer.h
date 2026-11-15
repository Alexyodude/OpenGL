#pragma once

#include <vector>
#include <cstdint>
#include <cassert>
#include "../framework/SimLayer.h"

struct HeatCell {
    float temperature;
    float alpha;
    float source;
};
static_assert(sizeof(HeatCell) == 12, "HeatCell must match GLSL std430 layout");

class HeatLayer : public SimLayer {
public:
    HeatLayer(int gridWidth, int gridHeight, int workgroupSize = 16)
        : SimLayer("Heat", gridWidth, gridHeight, workgroupSize) {}

    std::vector<ShaderPass> GetPasses() const override;
    void InitGrid(std::vector<uint8_t>& buffer, int parentCellIndex, const uint8_t* parentCellData) override;
    void BakeToParent(const std::vector<uint8_t>& buffer, uint8_t* parentCellData) override;
    void SetComputeUniforms(Shader& computeShader, int passIndex) override;
    int GetCellSizeBytes() const override { return sizeof(HeatCell); }
};
