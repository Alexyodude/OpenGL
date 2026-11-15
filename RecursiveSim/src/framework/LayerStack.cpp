#include "LayerStack.h"
#include "SimLayer.h"
#include "SSBOManager.h"
#include "../core/Log.h"

#include <cassert>
#include <sstream>

void LayerStack::PushRoot(SSBOManager& ssbo) {
    assert(m_Factory && "LayerStack: factory not set");
    assert(m_Stack.empty() && "LayerStack: PushRoot called with non-empty stack");

    LayerFrame frame;
    frame.layer = m_Factory(0);
    frame.parentCellIndex = -1;

    SimLayer* layer = frame.layer.get();
    layer->ValidateGridSize();

    // Initialize root grid (no parent data)
    layer->InitGrid(frame.cpuData, -1, nullptr);

    // Upload to SSBO[0]
    ssbo.Allocate(layer->GetTotalCells(), layer->GetCellSizeBytes());
    ssbo.Upload(frame.cpuData);

    m_Stack.push_back(std::move(frame));
}

void LayerStack::DrillDown(int cellIndex, SSBOManager& ssbo) {
    assert(!m_Stack.empty());
    assert(m_Factory);

    // 1. Download current layer's SSBO state to its cpuData
    ssbo.Download(m_Stack.back().cpuData);

    // 2. Get the parent cell pointer
    SimLayer* parentLayer = m_Stack.back().layer.get();
    int cellSize = parentLayer->GetCellSizeBytes();
    const uint8_t* parentCellData = m_Stack.back().cpuData.data() + (size_t)cellIndex * cellSize;

    // 3. Free current SSBOs (single active layer rule)
    ssbo.Deallocate();

    // 4. Create new child layer
    int newDepth = (int)m_Stack.size();
    LayerFrame frame;
    frame.layer = m_Factory(newDepth);
    frame.parentCellIndex = cellIndex;

    SimLayer* childLayer = frame.layer.get();
    childLayer->ValidateGridSize();
    childLayer->InitGrid(frame.cpuData, cellIndex, parentCellData);

    // 5. Allocate and upload child SSBOs
    ssbo.Allocate(childLayer->GetTotalCells(), childLayer->GetCellSizeBytes());
    ssbo.Upload(frame.cpuData);

    m_Stack.push_back(std::move(frame));
}

void LayerStack::BakeAndReturn(SSBOManager& ssbo) {
    assert(m_Stack.size() > 1 && "LayerStack: nothing to bake into");

    // 1. Download current layer state
    ssbo.Download(m_Stack.back().cpuData);

    // 2. Get parent frame and the cell index we drilled into
    LayerFrame& childFrame = m_Stack.back();
    int parentCellIndex = childFrame.parentCellIndex;
    LayerFrame& parentFrame = m_Stack[m_Stack.size() - 2];

    // 3. Bake child data into the parent cell
    int parentCellSize = parentFrame.layer->GetCellSizeBytes();
    uint8_t* parentCellPtr = parentFrame.cpuData.data() + (size_t)parentCellIndex * parentCellSize;
    childFrame.layer->BakeToParent(childFrame.cpuData, parentCellPtr);

    // 4. Free child SSBOs
    ssbo.Deallocate();

    // 5. Pop the child frame
    m_Stack.pop_back();

    // 6. Re-allocate parent SSBOs and re-upload (with baked data)
    SimLayer* parentLayer = m_Stack.back().layer.get();
    ssbo.Allocate(parentLayer->GetTotalCells(), parentLayer->GetCellSizeBytes());
    ssbo.Upload(m_Stack.back().cpuData);
}

int LayerStack::CellIndexFromMousePos(float mouseX, float mouseY,
    int windowWidth, int windowHeight) const
{
    if (m_Stack.empty()) return -1;

    SimLayer* layer = m_Stack.back().layer.get();
    int gw = layer->GetGridWidth();
    int gh = layer->GetGridHeight();

    // Map mouse [0, windowWidth] -> cell column, [0, windowHeight] -> cell row
    int col = (int)(mouseX / windowWidth * gw);
    int row = (int)(mouseY / windowHeight * gh);

    if (col < 0 || col >= gw || row < 0 || row >= gh)
        return -1;

    return row * gw + col;
}

SimLayer* LayerStack::GetActiveLayer() const {
    if (m_Stack.empty()) return nullptr;
    return m_Stack.back().layer.get();
}

std::vector<std::string> LayerStack::GetBreadcrumbs() const {
    std::vector<std::string> crumbs;
    for (size_t i = 0; i < m_Stack.size(); i++) {
        std::ostringstream oss;
        oss << m_Stack[i].layer->GetName();
        if (m_Stack[i].parentCellIndex >= 0)
            oss << " [" << m_Stack[i].parentCellIndex << "]";
        crumbs.push_back(oss.str());
    }
    return crumbs;
}
