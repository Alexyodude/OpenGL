#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cstdint>
#include <functional>

class SimLayer;
class SSBOManager;
class GridRenderer;
class Shader;

// Represents one level in the navigation stack.
struct LayerFrame {
    std::unique_ptr<SimLayer> layer;
    std::vector<uint8_t> cpuData;   // Baked CPU-side data for this layer
    int parentCellIndex = -1;        // Which cell in the parent was drilled into
};

// Manages the recursive navigation stack.
// DrillDown allocates a new sub-grid from a parent cell.
// BakeAndReturn aggregates the current sub-grid back to parent and pops.
class LayerStack {
public:
    using LayerFactory = std::function<std::unique_ptr<SimLayer>(int depth)>;

    LayerStack() = default;
    ~LayerStack() = default;

    // Set the factory that creates layers by depth (0 = top level).
    void SetLayerFactory(LayerFactory factory) { m_Factory = std::move(factory); }

    // Push the root layer (depth 0) onto the stack.
    void PushRoot(SSBOManager& ssbo);

    // Drill into cell at cellIndex of the current active layer.
    void DrillDown(int cellIndex, SSBOManager& ssbo);

    // Bake current layer back to parent cell and pop.
    void BakeAndReturn(SSBOManager& ssbo);

    // Convert a mouse position (in window pixels) to a flat cell index.
    int CellIndexFromMousePos(float mouseX, float mouseY, int windowWidth, int windowHeight) const;

    SimLayer* GetActiveLayer() const;
    int GetDepth() const { return (int)m_Stack.size(); }
    bool CanGoBack() const { return m_Stack.size() > 1; }

    // Breadcrumb strings for UI: e.g. ["Chemicals", "Heat [42]", "Particles [7]"]
    std::vector<std::string> GetBreadcrumbs() const;

private:
    std::vector<LayerFrame> m_Stack;
    LayerFactory m_Factory;
};
