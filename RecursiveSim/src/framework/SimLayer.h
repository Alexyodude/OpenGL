#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "ShaderPass.h"
#include "SSBOManager.h"

class Shader;
class GridRenderer;

// Abstract base class for simulation layers.
// Bake logic is virtual (physics-specific per layer).
// Compute dispatch is data-driven via GetPasses().
class SimLayer {
public:
    SimLayer(const std::string& name, int gridWidth, int gridHeight, int workgroupSize = 16)
        : m_Name(name), m_GridWidth(gridWidth), m_GridHeight(gridHeight), m_WorkgroupSize(workgroupSize) {}
    virtual ~SimLayer() = default;

    // --- Pure virtual interface (implement per layer) ---

    // Returns the ordered list of compute passes for one simulation step.
    virtual std::vector<ShaderPass> GetPasses() const = 0;

    // Initialize the sub-grid buffer from the parent cell's data.
    // parentCellData may be null for the top-level layer.
    virtual void InitGrid(std::vector<uint8_t>& buffer, int parentCellIndex, const uint8_t* parentCellData) = 0;

    // Aggregate the sub-grid back into the parent cell on zoom-out.
    virtual void BakeToParent(const std::vector<uint8_t>& buffer, uint8_t* parentCellData) = 0;

    // Set layer-specific uniforms on the compute shader for a given pass index.
    virtual void SetComputeUniforms(Shader& computeShader, int passIndex) = 0;

    // Returns sizeof(CellStruct) for this layer.
    virtual int GetCellSizeBytes() const = 0;

    // --- Concrete methods ---

    // Run nSteps simulation steps using the data-driven pass list.
    void Step(SSBOManager& ssbo, std::vector<Shader*>& shaders, int nSteps);

    // Render the current SSBO state via fullscreen quad.
    void Render(SSBOManager& ssbo, Shader& fragmentShader, GridRenderer& renderer,
                int viewportWidth, int viewportHeight);

    // Assert gridWidth and gridHeight are divisible by workgroupSize.
    void ValidateGridSize() const;

    // --- Accessors ---
    const std::string& GetName() const { return m_Name; }
    int GetGridWidth() const { return m_GridWidth; }
    int GetGridHeight() const { return m_GridHeight; }
    int GetWorkgroupSize() const { return m_WorkgroupSize; }
    size_t GetTotalCells() const { return (size_t)m_GridWidth * m_GridHeight; }

protected:
    std::string m_Name;
    int m_GridWidth;
    int m_GridHeight;
    int m_WorkgroupSize;
};
