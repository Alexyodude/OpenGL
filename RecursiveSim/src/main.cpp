#include <windows.h>
// Force discrete GPU on Optimus/hybrid systems
extern "C" {
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
}

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <cstring>

#include "core/Application.h"
#include "core/Shader.h"
#include "core/Log.h"
#include "framework/SSBOManager.h"
#include "framework/SimLayer.h"
#include "framework/LayerStack.h"
#include "framework/GridRenderer.h"
#include "framework/ShaderPass.h"
#include "layers/LayerRegistry.h"
#include "layers/BondLayer.h"
#include "ui/SimUI.h"

#include "imgui.h"

static const int WINDOW_WIDTH  = 1024;
static const int WINDOW_HEIGHT = 1024;

// Per-depth shader configuration
// Fragment shaders: one per depth level
static const char* FRAG_SHADERS[5] = {
    "res/shaders/fragment/chemicals_vis.frag",
    "res/shaders/fragment/heat_vis.frag",
    "res/shaders/fragment/particle_vis.frag",
    "res/shaders/fragment/bond_vis.frag",
    "res/shaders/fragment/test_vis.frag",     // depth 4: TestLayer (Game of Life)
};

// Compute shaders per depth (multiple entries = multiple passes)
// Depth 0 (Chemicals): no compute
// Depth 1 (Heat): 1 pass
// Depth 2 (Particles): 2 passes
// Depth 3 (Bonds): 1 pass
// Depth 4 (TestLayer): 1 pass
static const char* COMPUTE_SHADERS[5][2] = {
    { nullptr,                                    nullptr },
    { "res/shaders/compute/heat_step.comp",       nullptr },
    { "res/shaders/compute/particle_step1.comp",  "res/shaders/compute/particle_step2.comp" },
    { "res/shaders/compute/bond_step.comp",       nullptr },
    { "res/shaders/compute/test_step.comp",       nullptr },
};
static const int COMPUTE_PASS_COUNT[5] = { 0, 1, 2, 1, 1 };
static const int MAX_DEPTH = 5;

// Load (or reload) compute shaders for the given depth.
// Returns empty vector for static layers.
static std::vector<std::unique_ptr<Shader>> LoadComputeShaders(int depth) {
    std::vector<std::unique_ptr<Shader>> shaders;
    if (depth < 0 || depth >= MAX_DEPTH) return shaders;
    int n = COMPUTE_PASS_COUNT[depth];
    for (int i = 0; i < n; i++) {
        shaders.push_back(std::make_unique<Shader>(
            COMPUTE_SHADERS[depth][i], GL_COMPUTE_SHADER));
    }
    return shaders;
}

// Apply BondLayer edits: download SSBO, modify the cell, re-upload.
static void ApplyBondEdit(SSBOManager& ssbo, SimLayer* active,
                          const SimUI::State& uiState)
{
    if (!active) return;
    if (uiState.editCellIdx < 0) return;
    if ((size_t)uiState.editCellIdx >= active->GetTotalCells()) return;

    std::vector<uint8_t> data;
    ssbo.Download(data);

    BondCell* cells = reinterpret_cast<BondCell*>(data.data());
    BondCell& cell  = cells[uiState.editCellIdx];

    if (uiState.wantAddMass) {
        cell.mass += uiState.addMassAmount;
    }
    if (uiState.wantBreakBond) {
        cell.broken = 1.0f;
        cell.velocity     = glm::vec2(0.0f);
        cell.displacement = glm::vec2(0.0f);
    }

    ssbo.UploadToCurrent(data);
}

int main() {
    Application app(WINDOW_WIDTH, WINDOW_HEIGHT, "RecursiveSim");
    if (!app.Init()) return -1;

    // SSBO manager (single active layer at a time)
    SSBOManager ssbo;

    // Grid renderer (fullscreen quad)
    GridRenderer renderer;

    // Layer stack with registry factory
    LayerStack layerStack;
    layerStack.SetLayerFactory(LayerRegistry::Create);
    layerStack.PushRoot(ssbo);

    // Pre-load all fragment shaders (one per depth, vertex is shared)
    std::vector<std::unique_ptr<Shader>> fragmentShaders;
    for (int d = 0; d < MAX_DEPTH; d++) {
        fragmentShaders.push_back(std::make_unique<Shader>(
            "res/shaders/common/vertex.glsl",
            FRAG_SHADERS[d]));
    }

    // Compute shaders for current active depth (reloaded on DrillDown/BakeAndReturn)
    int activeDepth = 0;  // starts at Chemicals (depth 0)
    auto computeShaderOwners = LoadComputeShaders(activeDepth);
    std::vector<Shader*> computeShaderPtrs;
    for (auto& s : computeShaderOwners) computeShaderPtrs.push_back(s.get());

    // UI
    SimUI simUI;
    SimUI::State uiState;
    uiState.stepsPerFrame = 1;
    uiState.simRunning    = false;

    auto lastTime  = std::chrono::high_resolution_clock::now();
    int  frameCount = 0;
    float fps       = 0.0f;

    while (!app.ShouldClose()) {
        auto now = std::chrono::high_resolution_clock::now();
        float elapsed = std::chrono::duration<float>(now - lastTime).count();
        frameCount++;
        if (elapsed >= 1.0f) {
            fps = frameCount / elapsed;
            frameCount = 0;
            lastTime = now;
        }

        app.BeginFrame();

        // --- UI (fills uiState with intent for this frame) ---
        simUI.Render(layerStack, ssbo, WINDOW_WIDTH, WINDOW_HEIGHT, fps, uiState);

        // --- Process UI intent ---

        // Back (bake) button
        if (uiState.wantBake && layerStack.CanGoBack()) {
            layerStack.BakeAndReturn(ssbo);
            activeDepth = layerStack.GetDepth() - 1;
            computeShaderOwners = LoadComputeShaders(activeDepth);
            computeShaderPtrs.clear();
            for (auto& s : computeShaderOwners) computeShaderPtrs.push_back(s.get());
        }

        // Mouse click drill-down
        if (uiState.wantDrillDown && uiState.drillCellIdx >= 0) {
            int nextDepth = layerStack.GetDepth();
            if (LayerRegistry::HasLayer(nextDepth)) {
                layerStack.DrillDown(uiState.drillCellIdx, ssbo);
                activeDepth = layerStack.GetDepth() - 1;
                computeShaderOwners = LoadComputeShaders(activeDepth);
                computeShaderPtrs.clear();
                for (auto& s : computeShaderOwners) computeShaderPtrs.push_back(s.get());
                uiState.simRunning = false;  // Pause on layer switch
            }
        }

        // BondLayer edits (Add Mass / Break Bond)
        if (activeDepth == 3 && (uiState.wantAddMass || uiState.wantBreakBond)) {
            ApplyBondEdit(ssbo, layerStack.GetActiveLayer(), uiState);
        }

        // --- Simulate ---
        SimLayer* active = layerStack.GetActiveLayer();
        if (uiState.simRunning && active && !computeShaderPtrs.empty()) {
            active->Step(ssbo, computeShaderPtrs, uiState.stepsPerFrame);
        }

        // --- Render ---
        if (active && activeDepth < (int)fragmentShaders.size()) {
            active->Render(ssbo, *fragmentShaders[activeDepth], renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
        }

        app.EndFrame();
    }

    ssbo.Deallocate();
    return 0;
}
