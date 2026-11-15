#include "TestLayer.h"
#include "../core/Shader.h"
#include <cstdlib>
#include <cmath>

// Forward declaration of BondCell to seed from -- must stay in sync with BondLayer.h.
struct BondCell {
    float dispX, dispY;   // glm::vec2 displacement
    float velX, velY;     // glm::vec2 velocity
    float mass;
    float stiffness;
    float damping;
    float broken;
};

std::vector<ShaderPass> TestLayer::GetPasses() const {
    return {
        { "res/shaders/compute/test_step.comp", 0, 1, true }
    };
}

void TestLayer::InitGrid(std::vector<uint8_t>& buffer, int /*parentCellIndex*/, const uint8_t* parentCellData) {
    int totalCells = m_GridWidth * m_GridHeight;
    buffer.resize(totalCells * sizeof(TestCell), 0);
    TestCell* cells = reinterpret_cast<TestCell*>(buffer.data());

    // Seed ~30% random alive cells (classic random GoL start)
    for (int i = 0; i < totalCells; i++) {
        cells[i].alive    = (rand() % 10 < 3) ? 1.0f : 0.0f;
        cells[i].age      = 0.0f;
        cells[i].padding0 = 0.0f;
        cells[i].padding1 = 0.0f;
    }

    // If seeded from a parent BondCell, use broken state to bias alive probability
    if (parentCellData) {
        const BondCell* parent = reinterpret_cast<const BondCell*>(parentCellData);
        // High broken fraction -> more alive cells (chaotic start)
        if (parent->broken > 0.5f) {
            for (int i = 0; i < totalCells; i++) {
                cells[i].alive = (rand() % 10 < 6) ? 1.0f : 0.0f;
            }
        }
    }
}

void TestLayer::BakeToParent(const std::vector<uint8_t>& buffer, uint8_t* parentCellData) {
    if (!parentCellData) return;

    const TestCell* cells = reinterpret_cast<const TestCell*>(buffer.data());
    int totalCells = m_GridWidth * m_GridHeight;

    double sumAlive = 0.0;
    double sumAge   = 0.0;
    for (int i = 0; i < totalCells; i++) {
        sumAlive += cells[i].alive;
        sumAge   += cells[i].age;
    }

    float liveFrac = (totalCells > 0) ? static_cast<float>(sumAlive / totalCells) : 0.0f;
    float avgAge   = (totalCells > 0) ? static_cast<float>(sumAge   / totalCells) : 0.0f;

    // NaN guard
    if (!std::isfinite(liveFrac)) liveFrac = 0.0f;
    if (!std::isfinite(avgAge))   avgAge   = 0.0f;

    BondCell* parent = reinterpret_cast<BondCell*>(parentCellData);
    // Live fraction -> broken state (population collapse = bond breaking)
    parent->broken = 1.0f - liveFrac;
    // Average age -> stiffness (older patterns = more stable = stiffer)
    parent->stiffness = 0.5f + avgAge * 0.1f;
}

void TestLayer::SetComputeUniforms(Shader& computeShader, int /*passIndex*/) {
    computeShader.SetUniform1i("u_GridWidth",  m_GridWidth);
    computeShader.SetUniform1i("u_GridHeight", m_GridHeight);
}
