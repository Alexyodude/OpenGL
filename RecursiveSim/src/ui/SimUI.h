#pragma once

#include <string>
#include <vector>
#include <cstdint>

class LayerStack;
class SSBOManager;

// All ImGui panels for RecursiveSim.
// Owns no GL state -- purely reads framework state and emits UI.
class SimUI {
public:
    struct State {
        int  stepsPerFrame = 1;
        bool simRunning    = false;
        bool wantBake      = false;   // Back button pressed this frame
        bool wantDrillDown = false;   // Mouse cell click this frame
        int  drillCellIdx  = -1;      // Cell index to drill into

        // BondLayer editing (only valid when at bond depth)
        bool wantAddMass   = false;
        bool wantBreakBond = false;
        int  editCellIdx   = -1;      // Cell targeted by bond edit buttons
        float addMassAmount = 0.5f;
    };

    SimUI() = default;

    // Render all ImGui panels. Fills out `state` with user intent this frame.
    // windowWidth/windowHeight used for mouse->cell mapping.
    void Render(LayerStack& layerStack, SSBOManager& ssbo,
                int windowWidth, int windowHeight,
                float fps, State& state);

private:
    void RenderMainPanel(LayerStack& layerStack, float fps, State& state);
    void RenderBondEditPanel(LayerStack& layerStack, SSBOManager& ssbo, State& state);
    void RenderCellInfoPanel(LayerStack& layerStack, SSBOManager& ssbo,
                              int windowWidth, int windowHeight);

    // Hover cell index (updated each frame from mouse position)
    int m_HoverCellIdx = -1;
};
