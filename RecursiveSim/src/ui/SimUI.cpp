#include "SimUI.h"
#include "../framework/LayerStack.h"
#include "../framework/SimLayer.h"
#include "../framework/SSBOManager.h"
#include "../layers/ChemicalsLayer.h"
#include "../layers/HeatLayer.h"
#include "../layers/ParticleLayer.h"
#include "../layers/BondLayer.h"

#include "imgui.h"
#include <string>
#include <cstring>

// Helper: get layer depth name
static const char* DepthName(int depth) {
    switch (depth) {
        case 0: return "Chemicals";
        case 1: return "Heat";
        case 2: return "Particles";
        case 3: return "Bonds";
        default: return "Unknown";
    }
}

void SimUI::Render(LayerStack& layerStack, SSBOManager& ssbo,
                   int windowWidth, int windowHeight,
                   float fps, State& state)
{
    // Reset per-frame intent
    state.wantBake      = false;
    state.wantDrillDown = false;
    state.drillCellIdx  = -1;
    state.wantAddMass   = false;
    state.wantBreakBond = false;
    state.editCellIdx   = -1;

    // Update hover cell from mouse (before panels so info panel can use it)
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse) {
        float mx = io.MousePos.x;
        // Flip Y: ImGui origin top-left, grid origin bottom-left
        float my = (float)windowHeight - io.MousePos.y;
        m_HoverCellIdx = layerStack.CellIndexFromMousePos(mx, my, windowWidth, windowHeight);

        // Mouse click -> drill down
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && m_HoverCellIdx >= 0)
        {
            state.wantDrillDown = true;
            state.drillCellIdx  = m_HoverCellIdx;
        }
    } else {
        m_HoverCellIdx = -1;
    }

    RenderMainPanel(layerStack, fps, state);

    int depth = layerStack.GetDepth() - 1;
    if (depth == 3)  // Bond depth
        RenderBondEditPanel(layerStack, ssbo, state);

    RenderCellInfoPanel(layerStack, ssbo, windowWidth, windowHeight);
}

void SimUI::RenderMainPanel(LayerStack& layerStack, float fps, State& state)
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(320, 0), ImGuiCond_Once);
    ImGui::Begin("RecursiveSim");

    ImGui::Text("FPS: %.1f", fps);
    ImGui::Separator();

    // --- Breadcrumb ---
    auto crumbs = layerStack.GetBreadcrumbs();
    std::string breadcrumb;
    for (size_t i = 0; i < crumbs.size(); i++) {
        if (i > 0) breadcrumb += " > ";
        breadcrumb += crumbs[i];
    }
    ImGui::TextWrapped("Path: %s", breadcrumb.c_str());
    ImGui::Separator();

    // --- Active layer info ---
    SimLayer* active = layerStack.GetActiveLayer();
    if (active) {
        ImGui::Text("Layer: %s", active->GetName().c_str());
        ImGui::Text("Grid:  %dx%d  (workgroup %d)",
            active->GetGridWidth(), active->GetGridHeight(), active->GetWorkgroupSize());
    }
    ImGui::Separator();

    // --- Simulate controls ---
    bool hasPasses = active && !active->GetPasses().empty();

    if (!hasPasses) {
        ImGui::TextDisabled("(static layer -- no simulation)");
    } else {
        ImGui::SliderInt("Steps/frame", &state.stepsPerFrame, 1, 64);
        if (ImGui::Button(state.simRunning ? "Pause##sim" : "Simulate##sim"))
            state.simRunning = !state.simRunning;
        ImGui::SameLine();
        ImGui::TextDisabled("(%s)", state.simRunning ? "running" : "paused");
    }

    // --- Back / Bake ---
    if (layerStack.CanGoBack()) {
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
        if (ImGui::Button("Back (Bake to parent)")) {
            state.wantBake = true;
            state.simRunning = false;
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::TextDisabled("bakes %s -> %s",
            DepthName(layerStack.GetDepth() - 1),
            DepthName(layerStack.GetDepth() - 2));
    }

    // --- Drill-down hint ---
    if (layerStack.GetDepth() < 4) {
        ImGui::Separator();
        ImGui::TextDisabled("Click a cell to drill into %s",
            DepthName(layerStack.GetDepth()));
    } else {
        ImGui::Separator();
        ImGui::TextDisabled("(max depth reached)");
    }

    ImGui::End();
}

void SimUI::RenderBondEditPanel(LayerStack& layerStack, SSBOManager& ssbo, State& state)
{
    ImGui::SetNextWindowPos(ImVec2(10, 320), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(320, 0), ImGuiCond_Once);
    ImGui::Begin("Bond Editor");

    SimLayer* active = layerStack.GetActiveLayer();
    if (!active) { ImGui::End(); return; }

    int totalCells = (int)active->GetTotalCells();
    ImGui::Text("Bond layer: %d cells total", totalCells);
    ImGui::Separator();

    // Let user pick a cell index to edit
    static int editIdx = 0;
    ImGui::SliderInt("Cell index", &editIdx, 0, totalCells - 1);

    ImGui::SliderFloat("Add mass amount", &state.addMassAmount, 0.1f, 5.0f);

    if (ImGui::Button("Add Mass")) {
        state.wantAddMass = true;
        state.editCellIdx = editIdx;
    }
    ImGui::SameLine();
    if (ImGui::Button("Break Bond")) {
        state.wantBreakBond = true;
        state.editCellIdx = editIdx;
    }

    ImGui::Separator();
    ImGui::TextDisabled("Changes take effect next simulate step");

    ImGui::End();
}

void SimUI::RenderCellInfoPanel(LayerStack& layerStack, SSBOManager& /*ssbo*/,
                                 int /*windowWidth*/, int /*windowHeight*/)
{
    if (m_HoverCellIdx < 0) return;

    SimLayer* active = layerStack.GetActiveLayer();
    if (!active) return;

    ImGui::SetNextWindowPos(ImVec2(340, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(220, 0), ImGuiCond_Always);
    ImGui::Begin("Cell Info", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

    int idx = m_HoverCellIdx;
    int gw  = active->GetGridWidth();
    int gh  = active->GetGridHeight();
    ImGui::Text("Cell [%d]  (%d, %d)", idx, idx % gw, idx / gw);
    ImGui::TextDisabled("(hover -- data shown on next download)");

    // Note: we don't download the SSBO every frame for hover (too expensive).
    // The cell info panel shows coordinates only. Users can use the bond edit
    // panel for explicit cell inspection at bond depth.
    int depth = layerStack.GetDepth() - 1;
    ImGui::Text("Layer: %s", DepthName(depth));
    ImGui::Text("Grid size: %dx%d", gw, gh);

    ImGui::End();
}
