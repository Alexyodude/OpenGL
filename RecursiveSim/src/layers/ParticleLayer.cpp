#include "ParticleLayer.h"
#include "../core/Shader.h"
#include <cstring>
#include <cstdlib>
#include <cmath>

// Forward declaration of HeatCell to avoid circular include.
// Must stay in sync with HeatLayer.h.
struct HeatCell {
    float temperature;
    float alpha;
    float source;
};

std::vector<ShaderPass> ParticleLayer::GetPasses() const {
    // Two-pass: pass1 reads SSBO[0]->writes SSBO[1] (no swap),
    //           pass2 reads SSBO[1]->writes SSBO[0] (no swap).
    // Matches Working-Two-Memory-Buffer-Method binding order exactly.
    return {
        { "res/shaders/compute/particle_step1.comp", 0, 1, false },
        { "res/shaders/compute/particle_step2.comp", 1, 0, false }
    };
}

void ParticleLayer::InitGrid(std::vector<uint8_t>& buffer, int /*parentCellIndex*/, const uint8_t* parentCellData) {
    int totalCells = m_GridWidth * m_GridHeight;
    buffer.resize(totalCells * sizeof(ParticleCell), 0);

    ParticleCell* cells = reinterpret_cast<ParticleCell*>(buffer.data());

    float baseMass = 0.0f;
    float baseTemp = 0.5f;

    if (parentCellData) {
        const HeatCell* parent = reinterpret_cast<const HeatCell*>(parentCellData);
        baseMass = parent->source;    // source field seeds mass density
        baseTemp = parent->temperature;
    }

    // Seed sparse particles: roughly 5% density
    for (int i = 0; i < totalCells; i++) {
        bool hasMass = (rand() % 20 == 0);
        float m = hasMass ? (baseMass > 0.001f ? baseMass : 0.3f) : 0.0f;

        cells[i].mass     = m;
        cells[i].state2   = m;
        cells[i].repulse  = m;
        cells[i].extra    = 0.0f;
        cells[i].movement = glm::vec2(0.0f, 0.0f);

        // Seed velocity from temperature: hotter = more kinetic energy
        float speed = baseTemp * 0.5f;
        if (hasMass) {
            float vx = ((rand() % 500) - 250) * 0.01f * speed;
            float vy = ((rand() % 500) - 250) * 0.01f * speed;
            cells[i].velocity = glm::vec2(vx, vy);
        } else {
            cells[i].velocity = glm::vec2(0.0f, 0.0f);
        }
    }
}

void ParticleLayer::BakeToParent(const std::vector<uint8_t>& buffer, uint8_t* parentCellData) {
    if (!parentCellData) return;

    const ParticleCell* cells = reinterpret_cast<const ParticleCell*>(buffer.data());
    int totalCells = m_GridWidth * m_GridHeight;

    // Total kinetic energy -> parent temperature
    // Mass flow (avg |velocity| weighted by mass) -> parent source
    double kineticEnergy = 0.0;
    double totalMass = 0.0;

    for (int i = 0; i < totalCells; i++) {
        float m = cells[i].mass;
        float vx = cells[i].velocity.x;
        float vy = cells[i].velocity.y;
        kineticEnergy += 0.5 * m * (vx * vx + vy * vy);
        totalMass += m;
    }

    float avgKE = (totalCells > 0) ? static_cast<float>(kineticEnergy / totalCells) : 0.0f;
    float avgMass = (totalCells > 0) ? static_cast<float>(totalMass / totalCells) : 0.0f;

    // NaN guards: protect parent cell from corruption on degenerate grids
    if (!std::isfinite(avgKE))   avgKE   = 0.0f;
    if (!std::isfinite(avgMass)) avgMass = 0.0f;

    HeatCell* parent = reinterpret_cast<HeatCell*>(parentCellData);
    parent->temperature = avgKE;
    parent->source = avgMass;
}

void ParticleLayer::SetComputeUniforms(Shader& computeShader, int /*passIndex*/) {
    computeShader.SetUniform1i("u_GridWidth",  m_GridWidth);
    computeShader.SetUniform1i("u_GridHeight", m_GridHeight);
}
