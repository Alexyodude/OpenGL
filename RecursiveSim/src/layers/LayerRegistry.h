#pragma once

#include <memory>
#include "../framework/SimLayer.h"

// Factory that maps navigation depth to a concrete SimLayer.
// Depth 0 = ChemicalsLayer, 1 = HeatLayer, 2 = ParticleLayer, 3 = BondLayer.
// Adding a new layer requires only registering it here.
class LayerRegistry {
public:
    static std::unique_ptr<SimLayer> Create(int depth);
    static bool HasLayer(int depth);

    // Default grid sizes per depth
    static int GridWidth(int depth);
    static int GridHeight(int depth);
};
