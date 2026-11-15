#include "LayerRegistry.h"
#include "ChemicalsLayer.h"
#include "HeatLayer.h"
#include "ParticleLayer.h"
#include "BondLayer.h"
#include "TestLayer.h"

// Default configurable grid sizes per depth level.
// All must be divisible by the layer's workgroup size (default 16).
static const int GRID_SIZES[][2] = {
    { 128, 128 },  // depth 0: ChemicalsLayer
    { 128, 128 },  // depth 1: HeatLayer
    { 64,  64  },  // depth 2: ParticleLayer
    { 64,  64  },  // depth 3: BondLayer
    { 64,  64  },  // depth 4: TestLayer (Game of Life -- extensibility proof)
};
static const int NUM_DEPTHS = 5;

std::unique_ptr<SimLayer> LayerRegistry::Create(int depth) {
    int w = GridWidth(depth);
    int h = GridHeight(depth);
    switch (depth) {
        case 0: return std::make_unique<ChemicalsLayer>(w, h);
        case 1: return std::make_unique<HeatLayer>(w, h);
        case 2: return std::make_unique<ParticleLayer>(w, h);
        case 3: return std::make_unique<BondLayer>(w, h);
        case 4: return std::make_unique<TestLayer>(w, h);  // Extensibility test: one line added
        default: return nullptr;
    }
}

bool LayerRegistry::HasLayer(int depth) {
    return depth >= 0 && depth < NUM_DEPTHS;
}

int LayerRegistry::GridWidth(int depth) {
    if (depth < 0 || depth >= NUM_DEPTHS) return 64;
    return GRID_SIZES[depth][0];
}

int LayerRegistry::GridHeight(int depth) {
    if (depth < 0 || depth >= NUM_DEPTHS) return 64;
    return GRID_SIZES[depth][1];
}
