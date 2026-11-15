#include "BondLayer.h"
#include "../core/Shader.h"
#include "ParticleLayer.h"
#include <cstring>
#include <cmath>

BondLayer::BondLayer(int gridWidth, int gridHeight)
    : SimLayer("Bonds", gridWidth, gridHeight, 16) {}

std::vector<ShaderPass> BondLayer::GetPasses() const {
    return {
        { "res/shaders/compute/bond_step.comp", 0, 1, true }
    };
}

void BondLayer::InitGrid(std::vector<uint8_t>& buffer, int /*parentCellIndex*/, const uint8_t* parentCellData) {
    int totalCells = m_GridWidth * m_GridHeight;
    buffer.resize(totalCells * sizeof(BondCell), 0);
    BondCell* cells = reinterpret_cast<BondCell*>(buffer.data());

    float seedMass = 1.0f;
    glm::vec2 seedVelocity(0.0f, 0.0f);

    if (parentCellData) {
        const ParticleCell* parent = reinterpret_cast<const ParticleCell*>(parentCellData);
        seedMass = parent->mass > 0.0f ? parent->mass : 1.0f;
        // Initial displacement from parent velocity
        seedVelocity = parent->velocity;
    }

    for (int i = 0; i < totalCells; i++) {
        cells[i].displacement = glm::vec2(0.0f, 0.0f);
        cells[i].velocity     = seedVelocity * 0.1f;
        cells[i].mass         = seedMass;
        cells[i].stiffness    = 1.0f;
        cells[i].damping      = 0.05f;
        cells[i].broken       = 0.0f;
    }

    // Place a few pre-displaced cells to seed oscillation
    int cx = m_GridWidth / 2;
    int cy = m_GridHeight / 2;
    cells[cy * m_GridWidth + cx].displacement = glm::vec2(0.5f, 0.0f);
}

void BondLayer::BakeToParent(const std::vector<uint8_t>& buffer, uint8_t* parentCellData) {
    if (!parentCellData) return;

    const BondCell* cells = reinterpret_cast<const BondCell*>(buffer.data());
    int totalCells = m_GridWidth * m_GridHeight;

    double sumDispX = 0.0, sumDispY = 0.0;
    double sumStiffness = 0.0;
    double sumMass = 0.0;
    int brokenCount = 0;

    for (int i = 0; i < totalCells; i++) {
        sumDispX    += cells[i].displacement.x;
        sumDispY    += cells[i].displacement.y;
        sumStiffness += cells[i].stiffness;
        sumMass      += cells[i].mass;
        if (cells[i].broken > 0.5f) brokenCount++;
    }

    float n = (float)totalCells;
    float avgDispX    = (n > 0) ? (float)(sumDispX / n) : 0.0f;
    float avgDispY    = (n > 0) ? (float)(sumDispY / n) : 0.0f;
    float avgStiffness = (n > 0) ? (float)(sumStiffness / n) : 1.0f;
    float brokenFrac  = (n > 0) ? (float)brokenCount / n : 0.0f;

    // NaN guards: protect parent cell from corruption on degenerate grids
    if (!std::isfinite(avgDispX))    avgDispX    = 0.0f;
    if (!std::isfinite(avgDispY))    avgDispY    = 0.0f;
    if (!std::isfinite(avgStiffness)) avgStiffness = 1.0f;
    if (!std::isfinite(brokenFrac))  brokenFrac  = 0.0f;

    float totalMass = (n > 0) ? (float)(sumMass / n) : 0.0f;
    if (!std::isfinite(totalMass))   totalMass   = 0.0f;

    ParticleCell* parent = reinterpret_cast<ParticleCell*>(parentCellData);
    // Average displacement -> parent velocity
    parent->velocity = glm::vec2(avgDispX, avgDispY);
    // Broken fraction -> mass loss
    parent->mass = totalMass * (1.0f - brokenFrac * 0.5f);
    // Average stiffness -> repulse
    parent->repulse = avgStiffness;
}

void BondLayer::SetComputeUniforms(Shader& computeShader, int /*passIndex*/) {
    computeShader.SetUniform1i("u_GridWidth",  m_GridWidth);
    computeShader.SetUniform1i("u_GridHeight", m_GridHeight);
}
