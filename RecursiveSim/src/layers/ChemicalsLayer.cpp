#include "ChemicalsLayer.h"
#include "../core/Shader.h"
#include <cstring>
#include <cmath>

ChemicalsLayer::ChemicalsLayer(int gridWidth, int gridHeight)
    : SimLayer("Chemicals", gridWidth, gridHeight, 16) {}

std::vector<ShaderPass> ChemicalsLayer::GetPasses() const {
    // Static display layer -- no compute passes
    return {};
}

void ChemicalsLayer::InitGrid(std::vector<uint8_t>& buffer, int /*parentCellIndex*/, const uint8_t* /*parentCellData*/) {
    int totalCells = m_GridWidth * m_GridHeight;
    buffer.resize(totalCells * sizeof(ChemicalsCell), 0);
    ChemicalsCell* cells = reinterpret_cast<ChemicalsCell*>(buffer.data());

    // Generate a default colored grid with varied initial values
    for (int y = 0; y < m_GridHeight; y++) {
        for (int x = 0; x < m_GridWidth; x++) {
            int idx = y * m_GridWidth + x;
            float fx = (float)x / m_GridWidth;
            float fy = (float)y / m_GridHeight;

            cells[idx].temperature  = 0.3f + 0.4f * fx;
            cells[idx].density      = 0.2f + 0.6f * fy;
            cells[idx].bondStrength = 0.5f + 0.3f * std::sin(fx * 6.28f) * std::cos(fy * 6.28f);
            cells[idx].reactivity   = 0.1f + 0.4f * (fx + fy) * 0.5f;
        }
    }
}

void ChemicalsLayer::BakeToParent(const std::vector<uint8_t>& /*buffer*/, uint8_t* /*parentCellData*/) {
    // Top-level layer -- no parent to bake into
}

void ChemicalsLayer::SetComputeUniforms(Shader& /*computeShader*/, int /*passIndex*/) {
    // No compute shader for static layer
}
