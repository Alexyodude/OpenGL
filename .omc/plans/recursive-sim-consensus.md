# Recursive Simulation Framework - Implementation Plan (Consensus v2)

**Date:** 2026-04-08
**Status:** CONSENSUS APPROVED (Planner + Architect + Critic)
**Complexity:** HIGH
**Estimated scope:** ~25 files across new project structure

---

## 1. Requirements Summary

### Functional Requirements
- Generic recursive simulation framework for OpenGL compute shaders
- 4-layer physics emergence demo: Chemicals > Heat > Particles > Bonds
- Snapshot-and-Expand navigation: click cell to zoom into sub-grid
- Only ONE layer active on GPU at a time
- Bake-on-exit: zoom out triggers sub-grid aggregation into parent cell
- Local changes only: modifications affect only the clicked cell's parent
- Arbitrary depth support (not hardcoded to 4)
- Configurable grid sizes per layer
- ImGui + GLFW mouse click UI for v1

### Non-Functional Requirements
- OpenGL 4.3+ (compute shader support)
- GLFW, GLEW, C++17, GLSL 430, ImGui
- Visual Studio 2022 (v143 toolset) build system
- SSBO ping-pong buffer pattern for single-pass layers; fixed two-buffer dispatch for multi-pass layers
- Configurable workgroup sizes per layer (default 16x16)
- Grid dimensions MUST be divisible by workgroup size (validated at runtime)
- Torus wrapping via wrapIndex() pattern

### Acceptance Criteria (from spec)
1. Launch shows Chemicals grid with colored cells
2. Click cell -> Heat sub-grid with diffusion
3. Click heat cell -> Particles sub-grid with mass flow
4. Click particle cell -> Bonds sub-grid with bond simulation
5. Modify bonds (add mass, break bond)
6. Click Simulate -> runs N compute steps
7. Click Back -> bonds bake into particle properties
8. Back again -> particles bake into heat
9. Back again -> heat bakes into chemicals, cell visually changed
10. Framework supports adding layers without architecture changes

---

## 2. RALPLAN-DR Summary

### Guiding Principles
1. **Plugin-first layer design** - Adding a new simulation layer requires only: (a) a new compute shader, (b) a new C++ class inheriting from SimLayer, (c) registration in LayerRegistry. No framework code changes.
2. **Single active layer** - GPU resources (SSBOs, compute dispatch) allocated for exactly one layer at a time. Switching layers deallocates old and allocates new.
3. **Data flows through baking** - Parent cells are never directly modified by child simulations. Data moves strictly through explicit bake functions on zoom-out.
4. **Design around the hardest case** - The framework's dispatch model must handle the ParticleLayer's two-pass compute from the start, not treat it as a special case later.
5. **Minimal viable complexity** - v1 uses flat 2D grids, simple ImGui controls, and direct SSBO visualization. No camera system, no 3D, no multi-GPU.

### Decision Drivers (Top 3)
1. **Multi-pass extensibility** - Can the dispatch model handle layers with 1, 2, or N compute passes without per-layer overrides?
2. **GPU resource efficiency** - Only one layer's SSBOs should be in GPU memory at a time.
3. **Correctness of bake pipeline** - Data must survive the full drill-down/bake-up round trip without corruption.

### Viable Options

#### Option A: Hybrid Inheritance + Data-Driven Dispatch
Each layer is a C++ class inheriting from `SimLayer`. Bake logic is virtual (physics-specific). Compute dispatch is data-driven: each layer declares a `std::vector<ShaderPass>` describing its shader passes and binding configurations. The base class `Step()` iterates over the pass vector.

**Pros:**
- Virtual bake methods give physics-specific clarity
- Data-driven dispatch handles 1-pass and 2-pass layers uniformly
- Base `Step()` is concrete -- no per-layer dispatch boilerplate
- New layers only override `InitGrid()`, `BakeToParent()`, `GetPasses()`, `SetComputeUniforms()`

**Cons:**
- Slightly more complex base class than pure inheritance
- ShaderPass configuration requires understanding the binding model

#### Option B: Pure Data-Driven / ECS-style
Layers defined by configuration structs specifying cell byte size, shader paths, pass count, bake function pointers. Single generic `SimulationEngine` interprets definitions.

**Pros:**
- Maximum decoupling; layers are "data" not "code"
- Could support runtime-defined layers or hot-reload

**Cons:**
- Bake logic is physics-specific and resists generic expression without a scripting layer
- Debugging is harder (indirection through config)
- Over-engineered for 4 known layers with distinct physics
- No existing pattern in the codebase to build on

**Decision: Option A (Hybrid)**
Option B rejected because: (1) bake logic between layers is inherently physics-specific, (2) adds complexity without proportional benefit for v1, (3) data-driven dispatch within Option A already captures the extensibility benefit. The hybrid approach was synthesized from Architect review feedback identifying that pure inheritance cannot handle multi-pass compute dispatch.

---

## 3. Project Structure

```
RecursiveSim/
  RecursiveSim.sln
  RecursiveSim.vcxproj
  Dependencies/                         # Copy from existing OpenGL/Dependices/
    GLFW/
    glew-2.1.0/
  res/
    shaders/
      common/
        vertex.glsl                     # Fullscreen quad vertex shader
      compute/
        heat_step.comp                  # Heat diffusion (1 pass)
        particle_step1.comp             # Particle pass 1: velocity/state diffusion
        particle_step2.comp             # Particle pass 2: mass transport
        bond_step.comp                  # Bond spring-mass (1 pass)
      fragment/
        chemicals_vis.frag
        heat_vis.frag
        particle_vis.frag
        bond_vis.frag
  src/
    main.cpp                           # Entry point, window, main loop
    core/
      Application.h / .cpp            # GLFW, GLEW, ImGui lifecycle
      Shader.h / .cpp                 # Ported from Heat branch (supports compute)
      Log.h / .cpp                    # GL error checking macros
    framework/
      SimLayer.h / .cpp               # Abstract base: bake (virtual) + dispatch (data-driven)
      ShaderPass.h                    # Struct: shader path + binding configuration
      LayerStack.h / .cpp            # Navigation stack + bake orchestration
      SSBOManager.h / .cpp           # SSBO alloc/upload/download with explicit binding control
      GridRenderer.h / .cpp          # Fullscreen quad rendering
    layers/
      ChemicalsLayer.h / .cpp
      HeatLayer.h / .cpp
      ParticleLayer.h / .cpp
      BondLayer.h / .cpp
      LayerRegistry.h / .cpp         # Factory: depth -> SimLayer constructor
    ui/
      SimUI.h / .cpp                 # ImGui: breadcrumb, controls, cell info
  vendor/
    glm/
    imgui/
    stb_image/
```

---

## 4. Core Class Hierarchy and Data Flow

### 4.1 ShaderPass (Data-Driven Dispatch Config)

```cpp
struct ShaderPass {
    std::string shaderPath;     // Path to .comp file
    int readBinding;            // Which SSBO index to bind as input
    int writeBinding;           // Which SSBO index to bind as output
    bool swapAfter;             // Swap ping-pong indices after this pass?
};

// HeatLayer declares:     [{heat_step.comp, 0, 1, true}]           -- 1 pass, swap after
// ParticleLayer declares:  [{particle_step1.comp, 0, 1, false},    -- pass 1: read 0, write 1, no swap
//                           {particle_step2.comp, 1, 0, false}]    -- pass 2: read 1, write 0, no swap
// BondLayer declares:      [{bond_step.comp, 0, 1, true}]          -- 1 pass, swap after
```

### 4.2 SimLayer (Abstract Base)

```
SimLayer (abstract)
  Members:
    - std::string m_Name
    - int m_GridWidth, m_GridHeight
    - int m_WorkgroupSize             // Must divide grid dimensions evenly
    - std::vector<uint8_t> m_BakedData  // CPU-side storage when inactive

  Pure Virtual Methods:
    - std::vector<ShaderPass> GetPasses() const
        // Returns shader pass configuration for this layer
    - void InitGrid(std::vector<uint8_t>& buffer, int parentCellIndex, const uint8_t* parentCellData)
        // Initializes sub-grid from parent cell properties
    - void BakeToParent(const std::vector<uint8_t>& buffer, uint8_t* parentCellData)
        // Aggregates sub-grid into parent cell (called on zoom-out)
    - void SetComputeUniforms(Shader& computeShader, int passIndex)
        // Sets layer-specific uniforms (including grid dimensions)
    - int GetCellSizeBytes() const
        // Returns sizeof(CellStruct) for this layer

  Concrete Methods:
    - void Step(SSBOManager& ssbo, std::vector<Shader*>& shaders, int nSteps)
        // For each step 1..nSteps:
        //   For each ShaderPass from GetPasses():
        //     Bind shaders[passIndex], call SetComputeUniforms()
        //     ssbo.BindForCompute(pass.readBinding, pass.writeBinding)
        //     glDispatchCompute(gridWidth/workgroupSize, gridHeight/workgroupSize, 1)
        //     glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT)
        //     if (pass.swapAfter) ssbo.SwapBuffers()
    - void Render(SSBOManager& ssbo, Shader& fragment, GridRenderer& renderer)
    - void ValidateGridSize() const
        // ASSERT: gridWidth % workgroupSize == 0 && gridHeight % workgroupSize == 0
```

### 4.3 SSBOManager (Explicit Binding Control)

```
SSBOManager
  Members:
    - GLuint m_SSBOs[2]
    - int m_CurrentIndex      // 0 or 1
    - size_t m_BufferSize

  Methods:
    - void Allocate(size_t cellCount, size_t cellSize)
        // glGenBuffers(2, ...) + glBufferData with GL_DYNAMIC_COPY
    - void Upload(const std::vector<uint8_t>& data)
        // glBufferSubData to SSBO[0]
    - void Download(std::vector<uint8_t>& data)
        // glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT) <-- REQUIRED before readback
        // glGetBufferSubData from SSBO[m_CurrentIndex]
    - void BindForCompute(int readIdx, int writeIdx)
        // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBOs[readIdx])
        // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_SSBOs[writeIdx])
    - void BindForRender()
        // Both SSBOs bound for fragment shader read
    - void SwapBuffers()
        // m_CurrentIndex = (m_CurrentIndex + 1) % 2
    - void Deallocate()
        // glDeleteBuffers
```

### 4.4 Cell Structs

```cpp
// All structs use std430-compatible layout (floats and vec2-equivalents only)

struct HeatCell {
    float temperature;
    float alpha;
    float source;
};
static_assert(sizeof(HeatCell) == 12, "HeatCell must match GLSL std430 layout");

struct ParticleCell {
    glm::vec2 velocity;     // Use glm::vec2 to match existing branch pattern
    glm::vec2 movement;
    float mass;
    float repulse;
    float state2;
    float extra;             // Used by compute2 (NOT dead padding)
};
static_assert(sizeof(ParticleCell) == 32, "ParticleCell must match GLSL std430 layout");

struct BondCell {
    glm::vec2 displacement;
    glm::vec2 velocity;
    float mass;
    float stiffness;
    float damping;
    float broken;            // 0.0 = intact, 1.0 = broken
};
static_assert(sizeof(BondCell) == 32, "BondCell must match GLSL std430 layout");

struct ChemicalsCell {
    float temperature;
    float density;
    float bondStrength;
    float reactivity;
};
static_assert(sizeof(ChemicalsCell) == 16, "ChemicalsCell must match GLSL std430 layout");
```

### 4.5 Data Flow Diagram

```
[Launch] -> ChemicalsLayer grid (default colored, no prior bake data)
    |
    | Click cell [i]
    v
[DrillDown] -> Read ChemicalsCell[i] -> Init HeatLayer sub-grid
    |           (temperature seeds heat grid, alpha from density)
    |
    | Click cell [j] in heat grid
    v
[DrillDown] -> Read HeatCell[j] -> Init ParticleLayer sub-grid
    |           (temperature -> initial kinetic energy, source -> mass)
    |
    | Click cell [k] in particle grid
    v
[DrillDown] -> Read ParticleCell[k] -> Init BondLayer sub-grid
    |           (mass -> bond masses, velocity -> initial displacement)
    |
    | [Simulate N steps] -> multi-pass compute dispatch loop
    | [Modify] -> add mass, break bond via ImGui
    |
    | Click [Back]
    v
[BakeAndReturn] -> Aggregate BondLayer -> write ParticleCell[k]
    |   (avg displacement -> velocity, broken count -> mass loss,
    |    avg stiffness -> repulse)
    |
    | Click [Back]
    v
[BakeAndReturn] -> Aggregate ParticleLayer -> write HeatCell[j]
    |   (total kinetic energy -> temperature, mass flow -> source)
    |
    | Click [Back]
    v
[BakeAndReturn] -> Aggregate HeatLayer -> write ChemicalsCell[i]
    (avg temperature -> temperature, gradient -> reactivity)
```

**Note:** Sub-grid data is discarded after baking. Re-entering a cell generates a fresh sub-grid from the baked parent cell. This is an explicit v1 non-goal (no sub-grid caching).

---

## 5. Implementation Steps

### Step 1: Project Scaffolding and Core Infrastructure
**Files:** `RecursiveSim.sln`, `.vcxproj`, `src/main.cpp`, `src/core/Application.h/.cpp`, `src/core/Shader.h/.cpp`, `src/core/Log.h/.cpp`, `res/shaders/common/vertex.glsl`

**Work:**
- Create fresh VS2022 solution (v143, x64, OpenGL 4.3)
- Copy Dependencies/ (GLFW, GLEW) and vendor/ (glm, imgui, stb_image) from existing project
- Port `Shader.h/.cpp` from **Heat branch** (has compute shader support via `CreateSingleShader`; master branch Shader does NOT support compute)
- Port `Log.h/.cpp` from master
- Create Application class: GLFW window, GLEW, ImGui lifecycle, main loop skeleton
- Port fullscreen quad vertex.glsl from Heat branch
- Validate: check `GL_MAX_SHADER_STORAGE_BLOCK_SIZE` and `GL_MAX_COMPUTE_WORK_GROUP_COUNT` at startup

**Acceptance:**
- Project compiles and links
- Window opens with ImGui demo panel
- Shader class loads and compiles a trivial compute shader

---

### Step 2: Framework Core
**Files:** `src/framework/ShaderPass.h`, `SimLayer.h/.cpp`, `SSBOManager.h/.cpp`, `GridRenderer.h/.cpp`, `LayerStack.h/.cpp`

**Work:**
- Implement `ShaderPass` struct (shader path, read/write bindings, swapAfter flag)
- Implement `SSBOManager` with:
  - `BindForCompute(readIdx, writeIdx)` -- explicit binding control (NOT just current/other)
  - `Download()` with `glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT)` before readback
  - Allocate/Upload/SwapBuffers/Deallocate
- Implement `SimLayer` abstract base with:
  - Concrete `Step()` that iterates over `GetPasses()` vector, dispatching each pass with correct bindings
  - Concrete `Render()` for fullscreen quad visualization
  - `ValidateGridSize()` asserting grid dimensions divisible by workgroup size
  - Virtual: `GetPasses()`, `InitGrid()`, `BakeToParent()`, `SetComputeUniforms()`, `GetCellSizeBytes()`
- Implement `GridRenderer` for fullscreen quad drawing
- Implement `LayerStack` with DrillDown/BakeAndReturn/CellIndexFromMousePos

**Acceptance:**
- SSBOManager: upload test buffer, download, verify byte-for-byte match
- SSBOManager: BindForCompute(0,1) and BindForCompute(1,0) produce different binding configurations
- LayerStack: drill down and return with mock layer, verify data round-trips correctly
- ValidateGridSize: asserts fire for non-divisible dimensions

---

### Step 3: Implement the 4 Concrete Layers

Split into sub-steps because "porting" from monolithic Source.cpp files is actually "redesigning" with parameterized dimensions and framework integration.

#### Step 3a: HeatLayer (simplest -- proves single-pass dispatch)
**Files:** `src/layers/HeatLayer.h/.cpp`, `res/shaders/compute/heat_step.comp`, `res/shaders/fragment/heat_vis.frag`

**Work:**
- Adapt Heat branch `compute.shader`: replace hardcoded `gridWidth=1000`/`gridHeight=1000` with **uniforms**
- HeatCell struct with `static_assert(sizeof == 12)`
- `GetPasses()` returns single ShaderPass: `{heat_step.comp, 0, 1, true}`
- `InitGrid()`: seed temperature from parent ChemicalsCell
- `BakeToParent()`: average temperature across sub-grid
- Fragment shader: red channel = temperature / maxTemp

**Acceptance:**
- Heat diffusion runs on configurable grid (e.g., 128x128)
- Visual match to existing Heat branch behavior
- Struct alignment verified: upload known HeatCell array, download, compare

#### Step 3b: ParticleLayer (hardest -- proves multi-pass dispatch)
**Files:** `src/layers/ParticleLayer.h/.cpp`, `res/shaders/compute/particle_step1.comp`, `res/shaders/compute/particle_step2.comp`, `res/shaders/fragment/particle_vis.frag`

**Work:**
- Adapt Working-Two-Memory-Buffer-Method shaders:
  - `compute.shader` -> `particle_step1.comp`: replace hardcoded dimensions with uniforms
  - `compute2.shader` -> `particle_step2.comp`: replace hardcoded dimensions with uniforms
- ParticleCell struct with `glm::vec2` (matching existing branch) and `static_assert(sizeof == 32)`
- `GetPasses()` returns TWO passes:
  - `{particle_step1.comp, 0, 1, false}` -- pass 1: read SSBO[0], write SSBO[1], no swap
  - `{particle_step2.comp, 1, 0, false}` -- pass 2: read SSBO[1], write SSBO[0], no swap
  - This matches the existing branch's fixed binding order (ping-pong swap is intentionally disabled)
- `InitGrid()`: seed mass from parent HeatCell source, velocity from temperature gradient
- `BakeToParent()`: sum kinetic energy -> parent temperature

**Acceptance:**
- Two-pass compute dispatch works: pass 1 writes SSBO[1], pass 2 reads SSBO[1] and writes SSBO[0]
- Mass movement visible (particles flow)
- Verify multi-pass correctness: run 10 steps, download, check mass conservation (total mass before == total mass after, within float precision)

#### Step 3c: BondLayer (new compute shader)
**Files:** `src/layers/BondLayer.h/.cpp`, `res/shaders/compute/bond_step.comp`, `res/shaders/fragment/bond_vis.frag`

**Work:**
- New compute shader: spring-mass model `F = -k*displacement - d*velocity + neighborForces`
- BondCell struct with `static_assert(sizeof == 32)`
- `GetPasses()` returns single pass: `{bond_step.comp, 0, 1, true}`
- `InitGrid()`: place mass from parent ParticleCell, uniform stiffness
- `BakeToParent()`: avg displacement -> velocity, broken fraction -> mass loss, avg stiffness -> repulse

**Acceptance:**
- Bond simulation shows oscillation and damping
- Breaking bonds (setting broken=1.0) affects neighbors

#### Step 3d: ChemicalsLayer (static display) + LayerRegistry
**Files:** `src/layers/ChemicalsLayer.h/.cpp`, `res/shaders/fragment/chemicals_vis.frag`, `src/layers/LayerRegistry.h/.cpp`

**Work:**
- No compute shader (static layer)
- `GetPasses()` returns empty vector
- `InitGrid()`: generate default colored grid with sensible initial values
- `BakeToParent()`: no-op (top level)
- Fragment shader: maps {temperature, density, bondStrength, reactivity} to RGBA
- LayerRegistry: factory mapping depth -> layer constructor

**Acceptance:**
- Chemicals layer displays colored grid from default data
- LayerRegistry creates correct layer type for each depth

---

### Step 4: Navigation, Baking Pipeline, and UI Integration
**Files:** `src/ui/SimUI.h/.cpp`, modifications to `src/main.cpp`, `src/framework/LayerStack.cpp`

**Work:**
- Mouse click detection: GLFW callback -> check `ImGui::GetIO().WantCaptureMouse` -> LayerStack::CellIndexFromMousePos() -> DrillDown()
- Breadcrumb UI: ImGui panel showing "Chemicals > Cell[42] > Heat > Cell[7] > Particles"
- Control panel: simulate N steps slider+button, Back button, cell info on hover
- BondLayer editing: "Add Mass" and "Break Bond" buttons (download SSBO, modify cell, re-upload)
- Main loop: SimUI.Render() -> activeLayer->Step() -> activeLayer->Render() -> ImGui::Render() -> swapBuffers()
- Error handling: graceful message on shader compilation failure

**Acceptance:**
- Full drill-down: Chemicals -> Heat -> Particles -> Bonds
- Full bake-up: Bonds -> Particles -> Heat -> Chemicals with visible cell color change
- Bond modification reflected in simulation
- No GPU memory leaks (SSBOs deallocated on layer switch)
- ImGui clicks don't interfere with grid clicks

---

### Step 5: Polish, Edge Cases, and Extensibility Verification
**Files:** Various modifications

**Work:**
- Edge cases: clicks outside grid, empty cells, NaN protection in bake functions
- Configurable grid sizes per layer (verified divisible by workgroup size)
- wrapIndex boundary mode configurable per layer
- FPS display in ImGui
- **Extensibility test**: Add a 5th trivial layer (e.g., TestLayer with Game-of-Life rules). Verify only new files + one registration line needed.
- Memory safety: all SSBOs freed on exit, no dangling GL objects
- Assert glGenBuffers/glDeleteBuffers call counts match over 20 drill-down/bake cycles

**Acceptance:**
- Different grid sizes per layer work (e.g., Heat 128x128, Bonds 64x64)
- TestLayer added with zero framework changes
- No crashes on rapid drill-down/bake cycling
- Clean exit

---

## 6. ADR (Architecture Decision Record)

**Decision:** Hybrid inheritance + data-driven dispatch. SimLayer base with virtual bake methods and data-driven ShaderPass vector for compute dispatch.

**Drivers:**
1. Multi-pass extensibility -- ParticleLayer's 2-pass compute is the hardest case and must work without special-casing
2. GPU efficiency -- single layer active at a time
3. Proven patterns -- SSBO ping-pong and fixed two-buffer dispatch from existing branches

**Alternatives Considered:**
- Pure inheritance (concrete Step): Rejected because Step() cannot handle multi-pass dispatch without per-layer overrides, undermining the "no framework changes" principle.
- Pure data-driven/ECS: Rejected because bake logic is physics-specific and resists generic expression without a scripting layer.
- GPU-side baking (reduction compute shader): Deferred to v2. CPU readback simpler and sufficient for v1 grid sizes.
- All layers in GPU memory simultaneously: Rejected. Wastes VRAM, violates single-active-layer principle.

**Why Chosen:** The hybrid captures the best of both: OOP clarity for physics-specific bake logic, and data-driven flexibility for the dispatch model that must handle varying pass counts. Synthesized from Architect review feedback.

**Consequences:**
- Each new layer requires: C++ class + compute shader(s) + fragment shader + registration (3-5 files)
- Cell struct layout must be manually synchronized between C++ and GLSL (mitigated by static_assert)
- CPU readback adds latency on layer transitions (acceptable for v1 grid sizes; async PBO readback in v2)
- Sub-grid data is discarded after baking (no re-entry to previous state)

**Follow-ups:**
- v2: GPU-side baking via reduction compute shaders
- v2: Async PBO readback for large grids (>256x256)
- v2: Sub-grid state caching for re-entry
- v2: Hot-reload of compute shaders
- v2: Persistent save/load of world state

---

## 7. Risks and Mitigations

| Risk | Impact | Likelihood | Mitigation |
|------|--------|------------|------------|
| C++/GLSL struct alignment mismatch | Data corruption | HIGH | `static_assert` on every Cell struct size. Runtime validation: upload known data, download, compare. Use only floats and vec2 (avoid vec3 alignment trap). |
| GPU readback stalls (glGetBufferSubData) | Frame hitch on zoom | MEDIUM | Only on user click. `glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT)` before readback. v2: async PBO. |
| ParticleLayer two-pass compute correctness | Wrong simulation | HIGH | Explicit ShaderPass binding config: pass1 reads[0]->writes[1], pass2 reads[1]->writes[0]. Matches proven pattern from Working-Two-Memory-Buffer-Method. Test mass conservation. |
| Grid size not divisible by workgroup size | Silent cell skipping | MEDIUM | `ValidateGridSize()` asserts divisibility at layer creation. Document constraint. |
| ImGui mouse click conflicts with grid click | Wrong cell selected | MEDIUM | Check `ImGui::GetIO().WantCaptureMouse` before grid click processing. |
| NaN in bake aggregation (div by zero) | Corrupted parent cell | LOW | Guard divisions in BakeToParent. No undo in v1 -- document as limitation. |
| Shader compilation failure at runtime | App crash | LOW | Check compile/link status, display error in ImGui, abort layer gracefully. |

---

## 8. Verification Steps

1. **Struct alignment test**: Upload known HeatCell/ParticleCell/BondCell arrays, download, compare byte-for-byte. Run at startup in debug builds.
2. **Single-pass compute test**: Heat diffusion 100 steps on 128x128 with hot center. Download, verify temperature spread (center decreasing, edges increasing).
3. **Multi-pass compute test**: Particle simulation 10 steps. Download before and after. Verify total mass conserved (sum of all cell masses equal within float epsilon).
4. **Bake round-trip test**: Create HeatLayer sub-grid, run 50 steps, bake to parent ChemicalsCell. Verify parent temperature is non-zero and equals the sub-grid average.
5. **Full navigation test**: Drill Chemicals -> Heat -> Particles -> Bonds -> modify bond -> simulate -> Back -> Back -> Back. Verify top-level cell color changed.
6. **Extensibility test**: Add 5th TestLayer. Verify only new files + one LayerRegistry line. No framework modifications.
7. **Memory test**: 20 drill-down/bake cycles. Assert glGenBuffers count == glDeleteBuffers count. Monitor VRAM via task manager.
8. **Grid validation test**: Attempt to create a layer with grid 100x100, workgroup 16. Verify assertion fires (100 % 16 != 0).
9. **Multi-pass binding test**: After ParticleLayer Step(), verify SSBO[0] contains the final output (pass2 writes to binding 0).

---

## 9. Changelog

### v2 (Consensus revision)
Incorporated feedback from Architect and Critic reviews:
- **CRITICAL**: Redesigned `Step()` from concrete single-shader to concrete multi-pass via `ShaderPass` vector. ParticleLayer now declares 2 passes with explicit binding configurations.
- **CRITICAL**: `SSBOManager::BindForCompute()` now takes explicit (readIdx, writeIdx) parameters instead of assuming current/other.
- Added `glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT)` to `SSBOManager::Download()`
- Added `ValidateGridSize()` asserting grid dimensions divisible by workgroup size
- Added `static_assert` on all Cell struct sizes
- Removed false claim about master Layer class being abstract/virtual
- Changed Principle 4 from "Proven patterns over novelty" to "Design around the hardest case"
- Split Step 3 into sub-steps (3a-3d) with ParticleLayer explicitly called out as hardest
- Added mass conservation test for multi-pass verification
- Documented that ParticleCell.extra is NOT dead padding (used by compute2)
- Used `glm::vec2` in Cell structs instead of `float[2]` to match existing branch patterns
- Added ChemicalsLayer default initialization note (no prior bake data on first launch)
- Added sub-grid data discard note (re-entry generates fresh grid, not cached state)
- Added shader compilation error handling
- Added NaN protection in bake functions
