#include "HeatLayer.h"
#include "../core/Shader.h"
#include <cstring>
#include <cmath>

// Forward declaration of ChemicalsCell (defined in ChemicalsLayer.h)
// We redeclare it here to avoid a circular include. Must stay in sync.
struct ChemicalsCell {
    float temperature;
    float density;
    float bondStrength;
    float reactivity;
};

std::vector<ShaderPass> HeatLayer::GetPasses() const {
    return {
        { "res/shaders/compute/heat_step.comp", 0, 1, true }
    };
}

void HeatLayer::InitGrid(std::vector<uint8_t>& buffer, int /*parentCellIndex*/, const uint8_t* parentCellData) {
    int totalCells = m_GridWidth * m_GridHeight;
    buffer.resize(totalCells * sizeof(HeatCell), 0);

    HeatCell* cells = reinterpret_cast<HeatCell*>(buffer.data());

    float seedTemp = 0.5f;
    float seedAlpha = 0.1f;

    if (parentCellData) {
        const ChemicalsCell* parent = reinterpret_cast<const ChemicalsCell*>(parentCellData);
        seedTemp = parent->temperature;
        // alpha derived from density: denser material diffuses more slowly
        seedAlpha = 0.05f + 0.1f * (1.0f - parent->density);
    }

    for (int i = 0; i < totalCells; i++) {
        cells[i].temperature = seedTemp;
        cells[i].alpha = seedAlpha;
        cells[i].source = 0.0f;
    }

    // Place a hot spot at the center to seed diffusion
    int cx = m_GridWidth / 2;
    int cy = m_GridHeight / 2;
    int radius = m_GridWidth / 8;
    for (int y = cy - radius; y <= cy + radius; y++) {
        for (int x = cx - radius; x <= cx + radius; x++) {
            if (x >= 0 && x < m_GridWidth && y >= 0 && y < m_GridHeight) {
                cells[y * m_GridWidth + x].temperature = seedTemp + 1.0f;
                cells[y * m_GridWidth + x].source = 0.01f;
            }
        }
    }
}

void HeatLayer::BakeToParent(const std::vector<uint8_t>& buffer, uint8_t* parentCellData) {
    if (!parentCellData) return;

    const HeatCell* cells = reinterpret_cast<const HeatCell*>(buffer.data());
    int totalCells = m_GridWidth * m_GridHeight;

    double sumTemp = 0.0;
    double sumGradX = 0.0;
    double sumGradY = 0.0;

    for (int y = 0; y < m_GridHeight; y++) {
        for (int x = 0; x < m_GridWidth; x++) {
            int idx = y * m_GridWidth + x;
            sumTemp += cells[idx].temperature;

            // Compute gradient magnitude for reactivity
            int right = (x + 1) % m_GridWidth;
            int down = (y + 1) % m_GridHeight;
            float gx = cells[y * m_GridWidth + right].temperature - cells[idx].temperature;
            float gy = cells[down * m_GridWidth + x].temperature - cells[idx].temperature;
            sumGradX += gx * gx;
            sumGradY += gy * gy;
        }
    }

    float avgTemp = (totalCells > 0) ? static_cast<float>(sumTemp / totalCells) : 0.0f;
    float avgGradMag = (totalCells > 0)
        ? static_cast<float>((sumGradX + sumGradY) / totalCells)
        : 0.0f;
    // Clamp reactivity 0..1
    float reactivity = avgGradMag / (avgGradMag + 1.0f);

    // NaN guards: protect parent cell from corruption on degenerate grids
    if (!std::isfinite(avgTemp))   avgTemp   = 0.0f;
    if (!std::isfinite(reactivity)) reactivity = 0.0f;

    ChemicalsCell* parent = reinterpret_cast<ChemicalsCell*>(parentCellData);
    parent->temperature = avgTemp;
    parent->reactivity = reactivity;
}

void HeatLayer::SetComputeUniforms(Shader& computeShader, int /*passIndex*/) {
    computeShader.SetUniform1i("u_GridWidth", m_GridWidth);
    computeShader.SetUniform1i("u_GridHeight", m_GridHeight);
}
