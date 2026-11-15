# Deep Interview Spec: Recursive Simulation Framework

## Metadata
- Rounds: 16
- Final Ambiguity Score: 14%
- Type: brownfield (fresh codebase, referencing existing OpenGL compute shader branches)
- Generated: 2026-04-08
- Threshold: 20%
- Status: PASSED

## Clarity Breakdown
| Dimension | Score | Weight | Weighted |
|-----------|-------|--------|----------|
| Goal Clarity | 0.90 | 35% | 0.32 |
| Constraint Clarity | 0.85 | 25% | 0.21 |
| Success Criteria | 0.85 | 25% | 0.21 |
| Context Clarity | 0.80 | 15% | 0.12 |
| **Total Clarity** | | | **0.86** |
| **Ambiguity** | | | **14%** |

## Goal
Build a **generic recursive simulation framework** for OpenGL with GPU compute shaders, then implement a **4-layer physics emergence demo** on top of it. The framework supports arbitrary-depth recursive simulation where each grid cell can be "zoomed into" to reveal a sub-grid with its own simulation rules. Layers are simulated on-demand (when the user is interacting with them) and **baked to static properties** when the user zooms out. Changes propagate upward through baking -- they do NOT propagate to all cells of the same type, only the specific cell that was interacted with.

## Architecture: Snapshot-and-Expand Pattern
1. User sees the top-level grid (static/baked state)
2. User clicks a cell to zoom in -- the cell expands into its own sub-grid
3. The sub-grid runs a simulation with layer-specific rules (compute shaders)
4. User can interact (modify cells) and trigger simulation steps
5. User zooms out -- the sub-grid is **baked** into summary properties for the parent cell
6. The parent grid now reflects the baked change in that one cell
7. This pattern is recursive: any sub-grid cell can itself be zoomed into

Only ONE layer is actively simulated at a time (the one the user is interacting with). All other layers are static/baked. This means only one layer's SSBO needs to be on GPU at any time.

## The 4-Layer Physics Demo (v1)

| Layer | Depth | Simulation Rules | Bakes Into |
|-------|-------|-----------------|------------|
| **Chemicals** | 0 (top) | Color-mapped display of baked heat/particle/bond results | N/A (top level) |
| **Heat** | 1 | Heat diffusion (Fourier's law, simplified) | Temperature, thermal gradient -> Chemical properties |
| **Particles** | 2 | Mass flow / particle dynamics | Density, velocity, pressure -> Heat source terms |
| **Bonds** | 3 (deepest) | Bond/structure simulation (spring-mass or lattice inspired) | Stiffness, connectivity, mass distribution -> Particle constraints |

Rules at each layer are **physically-inspired but simplified** -- grounded in real physics concepts, discretized for GPU grid computation. Not quantitatively accurate, but producing recognizable physical behavior.

## Constraints
- **Fresh codebase** -- new project structure, referencing patterns from existing branches (SSBO ping-pong, compute shader dispatch, cell structures)
- **GPU compute shaders** -- all simulation runs on GPU via compute shaders with SSBOs
- **One active layer at a time** -- only the focused layer is on GPU, others are baked/static in CPU memory
- **Arbitrary depth support** -- the framework architecture must not hardcode layer count; the 4-layer demo is the first use case
- **Bake-on-exit** -- leaving a layer triggers baking; parent shows static baked result
- **Local changes only** -- modifying one cell at layer N only affects that cell's parent at layer N-1, not all cells
- **Pluggable rules** -- each layer has its own compute shader / rule set; the framework dispatches the correct one based on current layer
- **Configurable grid sizes per layer** -- each layer definition specifies its sub-grid resolution (e.g., bonds 64x64, particles 128x128, heat 256x256)
- **UI: ImGui + GLFW for v1** -- ImGui panels for controls (simulate, back, layer indicator), GLFW mouse callback for grid cell selection. Custom OpenGL UI deferred to future version
- **OpenGL 4.3+** -- required for compute shaders

## Non-Goals
- Real-time simulation of all layers simultaneously
- Quantitatively accurate physics (not a scientific simulator)
- Infinite zoom (practical limit exists, but architecture doesn't hardcode depth)
- Multiplayer or networking
- Saving/loading world state to disk (can be added later)

## Acceptance Criteria
- [ ] Launch app: see a top-level grid (Chemicals layer) with colored cells representing baked chemical properties
- [ ] Click any cell: zoom into Heat layer for that cell -- see a sub-grid with heat diffusion running
- [ ] Click any heat cell: zoom into Particles layer -- see mass flow simulation
- [ ] Click any particle cell: zoom into Bonds layer -- see bond/structure simulation (deepest level)
- [ ] Modify something at the Bonds level (e.g., add mass, break a bond)
- [ ] Click 'Simulate' -- bonds layer runs N steps of compute shader
- [ ] Click 'Back' -- bonds bake into particle properties, see the change reflected in the particle grid
- [ ] Click 'Back' again -- particles bake into heat properties, change propagates up
- [ ] Click 'Back' again -- heat bakes into chemicals, original top-level cell is now visually different
- [ ] Framework supports adding more layers without architectural changes

## Assumptions Exposed & Resolved
| Assumption | Challenge | Resolution |
|------------|-----------|------------|
| "Recursively simulated" means fractal-like infinite depth | Asked: chunk-based LOD vs fractal vs snapshot-expand? | Snapshot-and-expand: simulate locally, bake, zoom out |
| Physics from first principles (string theory, 4 forces) | Contrarian: this is unsolved physics, impossible on GPU | Emergent from simple rules: physically-inspired but simplified |
| All layers simulated simultaneously | Asked about GPU memory strategy | One active layer at a time, user-driven navigation |
| Generic framework OR physics demo | Ontologist: what IS this project? | A then B: generic framework first, physics demo on top |
| Rules need to be placeholder vs accurate | Asked about "meaningful" definition | In between: physically-inspired, recognizable behavior, not exact equations |
| Changes propagate globally | User clarified: local only | Changes only affect the specific interacted cell after baking |

## Technical Context
- **Existing codebase patterns to reference:**
  - SSBO ping-pong buffer pattern (all simulation branches)
  - Compute shader dispatch with workgroups (16x16 or 25x25)
  - Cell structures with multiple properties (Working-Two-Memory-Buffer-Method)
  - Heat diffusion compute shader (Heat branch)
  - Mass flow / particle dynamics (Working-Two-Memory-Buffer-Method)
  - `wrapIndex()` for torus wrapping
  - Fragment shader reading SSBOs for visualization
- **New code required:**
  - Layer management system (stack-based navigation)
  - Cell click detection (mouse -> grid coordinate mapping)
  - Baking pipeline (reduce sub-grid to parent cell properties)
  - Per-layer compute shader dispatch
  - Zoom in/out transitions
  - UI for simulate/back buttons (ImGui or custom)
- **Technology:** OpenGL 4.3+, GLFW, GLEW, C++, GLSL compute shaders

## Ontology (Key Entities)

| Entity | Type | Fields | Relationships |
|--------|------|--------|---------------|
| Recursive Framework | core domain | layer_stack, active_layer, rule_registry | Manages all layers, dispatches rules |
| Layer Stack | mechanism | layers[], current_depth, max_depth | Contains ordered layers, tracks navigation |
| Rule Plugin | mechanism | compute_shader, cell_struct, bake_function | Each layer has one, defines simulation behavior |
| Interactive Cell | UI | grid_position, parent_layer, child_grid | User clicks to zoom in, contains sub-grid |
| Bake-on-Exit | mechanism | source_grid, target_cell, reduction_function | Triggered on zoom-out, reduces sub-grid to parent properties |
| Bonds Layer | scale (depth 3) | stiffness, connectivity, mass | Deepest, bakes into particle constraints |
| Particles Layer | scale (depth 2) | mass, velocity, density, pressure | Bakes into heat source terms |
| Heat Layer | scale (depth 1) | temperature, thermal_conductivity, source | Bakes into chemical properties |
| Chemicals Layer | scale (depth 0) | color, composition, state | Top level, display only |

## Ontology Convergence

| Round | Entities | New | Changed | Stable | Stability |
|-------|----------|-----|---------|--------|-----------|
| 1 | 3 | 3 | - | - | N/A |
| 2 | 5 | 2 | 0 | 3 | 60% |
| 3 | 8 | 5 | 1 | 2 | 38% |
| 4 | 6 | 2 | 1 | 3 | 50% |
| 5 | 7 | 1 | 0 | 6 | 86% |
| 6 | 7 | 1 | 0 | 6 | 86% |
| 7 | 10 | 5 | 1 | 4 | 57% |
| 8 | 7 | 1 | 2 | 4 | 70% |
| 9 | 9 | 4 | 0 | 5 | 71% |
| 10 | 9 | 0 | 0 | 9 | 100% |
| 11 | 11 | 2 | 0 | 9 | 82% |
| 12 | 11 | 0 | 0 | 11 | 100% |
| 13 | 11 | 0 | 0 | 11 | 100% |
| 14 | 11 | 0 | 0 | 11 | 100% |

Ontology fully converged from round 12 onward.

## Interview Transcript
<details>
<summary>Full Q&A (14 rounds)</summary>

### Round 1
**Q:** When you say 'recursively simulated world', what IS the core simulation? Is this building on one of your existing branches, or is it a new type of simulation? And by 'recursively' - do you mean chunk-based LOD, fractal/scale-recursive, or snapshot-and-expand?
**A:** Snapshot-and-expand
**Ambiguity:** 100% -> 83%

### Round 2
**Q:** Walk me through one concrete cycle. What rules run in the small region? What properties get baked? What does the larger-scale simulation do with baked values?
**A:** Multi-property but the onion layer goes deeper where each property is simulated further until everything is unified into some mathematical high-dimensional geometric approach.
**Ambiguity:** 83% -> 81%

### Round 3
**Q:** How many layers deep? Is the unified geometric approach = each cell as a point in N-dimensional property space?
**A:** Each property determined in the simulation will be simulated via four fundamental forces, and below that will be string theory.
**Ambiguity:** 81% -> 85% (scope expanded)

### Round 4 (CONTRARIAN)
**Q:** Simulating four fundamental forces from first principles is an unsolved problem in physics. What if the goal is emergent behaviors from simple rules rather than top-down simulation of known physics?
**A:** Emergent from simple rules.
**Ambiguity:** 85% -> 70%

### Round 5
**Q:** What would you SEE that would make you say 'it's working'? Particle clustering? Property diffusion? Self-organizing patterns? Or just the framework?
**A:** Just the framework first.
**Ambiguity:** 70% -> 57%

### Round 6 (SIMPLIFIER)
**Q:** For minimum viable recursive framework, is 2 layers enough, or is arbitrary depth the point?
**A:** Arbitrary depth is the point.
**Ambiguity:** 57% -> 51%

### Round 7
**Q:** What's the acceptance test for arbitrary-depth recursion?
**A:** In terms of mass: energy to subatomic particles to atoms to chemicals.
**Ambiguity:** 51% -> 53% (scope re-expanded with specific layers)

### Round 8 (ONTOLOGIST)
**Q:** What IS this project really? A generic recursive framework (A), a physics emergence simulator (B), or A then B?
**A:** A then B.
**Ambiguity:** 53% -> 46%

### Round 9
**Q:** GPU memory strategy: all layers on GPU, active layer only, or hybrid?
**A:** Not all layers simulated at same time. Each layer is simulated when user is interacting. Grid spaces are interactable -- clicking enters a sub-grid. Leaving requires save/simulate. Changes don't propagate to all blocks of that type, only the interacted part.
**Ambiguity:** 46% -> 37%

### Round 10
**Q:** Build on existing compute shader code, start fresh referencing old, or extend master?
**A:** Start fresh, reference old.
**Ambiguity:** 37% -> 31%

### Round 11
**Q:** Proposed 9-step acceptance test with 3-layer zoom demo. Complete?
**A:** Close but need more. Zooming out should bake. Below heat should be particles, below that bonds and structure, etc.
**Ambiguity:** 31% -> 29%

### Round 12
**Q:** For v1, can layer rules be simple placeholders, semi-realistic, or must be meaningful?
**A:** Must be meaningful.
**Ambiguity:** 29% -> 25%

### Round 13
**Q:** Is "meaningful" = recognizable behavior, real equations, or somewhere in between?
**A:** Somewhere in between.
**Ambiguity:** 25% -> 21%

### Round 14
**Q:** Final 9-step acceptance test: Chemicals -> Heat -> Particles -> Bonds, with bake-on-exit at each level. Complete?
**A:** Yes, that's v1 done.
**Ambiguity:** 21% -> 17%

### Round 15
**Q:** For the UI (cell clicking, zoom, simulate/back buttons) -- ImGui + GLFW clicks, custom OpenGL UI, or ImGui for v1 and custom later?
**A:** ImGui for v1, custom later.
**Ambiguity:** 17% -> 15%

### Round 16
**Q:** What grid resolution per layer? Uniform 128x128, uniform 256x256, configurable per layer, or don't care yet?
**A:** Configurable per layer.
**Ambiguity:** 15% -> 14%
</details>
