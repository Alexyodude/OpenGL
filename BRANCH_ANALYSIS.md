# OpenGL Project - Branch Analysis

## Overview

This repository tracks the evolution of an OpenGL project from a basic 2D textured-quad renderer into increasingly sophisticated GPU-based simulations. The branches represent distinct experiments, each building on shared foundations but diverging in purpose and complexity.

---

## Branch Map

```
master (0bcbe54)
├── branch-practice ─── gravity, WASD player, edge wrapping (CPU-based)
│
├── Two-Texture-Method-4x4-Pixel-Structure ─── Conway's Game of Life (GPU compute + fragment)
│   │
│   ├── Working-Two-Memory-Buffer-Method ─── velocity-based mass flow (dual compute shaders)
│   │   │
│   │   ├── Changing-Attraction-Mechanic-to-Directional ─── gravity matrix directional attraction
│   │   │
│   │   └── Heat ─── heat diffusion PDE solver
│
└── Yay ─── (identical to master)
```

---

## Branch-by-Branch Analysis

### 1. master

**Purpose:** Foundation rendering framework.

**What it does:**
- 2D textured-quad renderer using OpenGL 3.3+ core profile
- Clean abstraction layer: VertexArray, VertexBuffer, IndexBuffer, Shader, Texture, Renderer
- Layer/SandBox architecture for scene management
- Multi-texture support (up to 3 textures via texture index per vertex)
- Orthographic projection (1000x500 viewport)
- WASD camera panning via Layer input handling
- ImGui integration for debug UI

**Key files:**
- `OpenGL/src/Source.cpp` - GLFW window setup, main loop
- `OpenGL/src/SandBox.cpp/h` - Scene graph, quad generation, rendering
- `OpenGL/src/Layer.cpp/h` - Input dispatch, scene wrapper
- `OpenGL/res/shaders/Basic.shader` - Multi-texture vertex/fragment shader

**Assessment:** Solid foundation. The abstractions (VAO/VBO/IBO wrappers) follow standard modern OpenGL patterns. Good starting point for experimentation.

---

### 2. branch-practice

**Purpose:** CPU-based 2D wave/particle simulation with interactive player.

**What it does:**
- 500x500 backend grid with wave propagation physics
- Player character at grid center with WASD movement
- Wave equation with dispersion, mass-based repulsion, diagonal neighbor contributions
- Momentum transfer between particles (mass-ratio based velocity exchange)
- Velocity threshold movement system (particles slide when accumulated velocity > sqrt(2)/2)
- Toroidal edge wrapping
- ImGui-exposed tuning parameters (dispersion, repulsion, intensity, well strength)

**Key additions over master:**
- `Player.cpp/h` - Player entity with position tracking
- `SandBox.cpp` expanded to ~816 lines with full simulation logic
- `ShaderSource.h` - Inline shader definitions

**Assessment:** 
- The wave equation approach is valid for fluid-like behavior
- **Main concern: entirely CPU-bound.** A 500x500 grid with 8-neighbor stencil calculations per cell per frame will bottleneck quickly. This is exactly the kind of workload that benefits from GPU compute shaders
- The momentum transfer system (mass-ratio based) is physically reasonable
- Edge wrapping implementation is correct (toroidal topology)
- Good that tuning parameters are exposed in ImGui for experimentation

**Verdict: Valid approach, but CPU execution is the bottleneck. The later branches correctly address this by moving to GPU compute.**

---

### 3. Two-Texture-Method-4x4-Pixel-Structure

**Purpose:** Conway's Game of Life on the GPU using dual SSBOs.

**What it does:**
- 1000x1000 grid simulated entirely on GPU
- Dual Shader Storage Buffer Objects (SSBOs) in ping-pong pattern
- Cell structure: `{ float mass, float state2 }`
- Compute shader (16x16 workgroups) handles diffusion: `state2 = neighborSum * 0.01 + mass`
- Fragment shader reads SSBOs directly for rendering
- Update.shader implements Conway's birth/death rules
- Separate Render.shader for visualization

**Architecture shift from master:**
- Removed all CPU-side abstraction classes (Renderer, VertexArray, VertexBuffer, etc.)
- Source.cpp handles everything directly - buffer creation, shader dispatch, rendering
- Multiple specialized shaders replace the single Basic.shader

**Assessment:**
- The SSBO ping-pong pattern is the standard approach for GPU cellular automata - correct choice
- 16x16 workgroup size is reasonable (256 threads, good occupancy)
- Removing the old abstractions was the right call - they were designed for CPU-driven rendering, not GPU compute pipelines
- Fragment shader doing computation alongside rendering is a valid shortcut but mixes concerns

**Verdict: Valid. Standard GPU compute approach for cellular automata. Clean transition from CPU to GPU pipeline.**

---

### 4. Working-Two-Memory-Buffer-Method

**Purpose:** Extended dynamics with velocity-based mass flow.

**What it does:**
- Richer cell structure: `{ vec2 velocity, vec2 movement, float mass, float repulse, float state2, float test1 }`
- Two-pass compute pipeline:
  - **Pass 1 (compute.shader):** Calculate velocity from state/repulse gradients, diffusion (`massSum * 0.12 + mass`), repulse propagation (`repulseSum * 0.07 + 2.0 * mass`)
  - **Pass 2 (compute2.shader):** Directional mass transfer based on movement vectors, conditional flow merging, weighted velocity from incoming neighbors
- 25x25 workgroups (625 threads)
- Torus wrapping via `wrapIndex()`

**Key improvements over Two-Texture:**
- Velocity and movement vectors enable directional flow (not just diffusion)
- Repulsion mechanism prevents mass collapse
- Two-pass design separates force calculation from mass movement - physically correct (you shouldn't move mass while still computing forces)
- `step()` function usage for branch-free GPU conditionals

**Assessment:**
- The two-pass separation (calculate forces, then move mass) is the correct approach for this type of simulation. Doing both in one pass would create race conditions and order-dependent results
- 25x25 workgroups = 625 threads, which is above the typical 256 sweet spot. This may reduce occupancy on some GPUs. Consider 16x16 (256) or 32x32 (1024, if supported)
- The velocity-threshold movement system is a discrete approximation of continuous flow - valid for the grid-based approach
- Repulsion mechanism prevents singularities (all mass collapsing to one cell) - important for stability

**Verdict: Valid and well-structured. The dual-pass approach is correct for this class of simulation.**

---

### 5. Changing-Attraction-Mechanic-to-Directional

**Purpose:** Gravity field simulation with directional matrices.

**What it does:**
- Most complex cell structure: `{ vec2 velocity, vec2 movement, mat4 gravity, float a, float mass, float repulse, float extra }`
- The mat4 stores a 3x3 directional gravity field (packed into the mat4):
  ```
  gravity[0][0]  gravity[0][1]  gravity[0][2]
  gravity[1][0]  gravity[1][1]  gravity[1][2]
  gravity[2][0]  gravity[2][1]  gravity[2][2]
  ```
- Directional weighting: cardinal neighbors at 0.5, diagonal at 0.25
- Gravity field propagation with 0.99 decay efficiency
- Velocity derived from gravity gradients instead of simple state differences
- Mass movement follows `sign(movement)` direction with complex conditional logic

**Key evolution from Working-Two-Memory-Buffer:**
- Replaces scalar state/repulse gradients with a full directional gravity tensor
- Gravity field persists and propagates through space (not just local neighbor differences)
- Enables long-range attraction patterns

**Assessment:**
- Using a mat4 to store a 3x3 directional field wastes 7 floats per cell. A mat3 or even a vec4 + float arrangement would save ~28 bytes per cell. At 1M cells, that's ~28MB of wasted GPU memory
- The gravity propagation concept is valid - it's essentially a discrete approximation of a gravitational potential field
- The 0.99 decay factor means gravity influence drops to ~37% after 100 cells - reasonable for local interactions
- The directional weighting (0.5 cardinal, 0.25 diagonal) correctly accounts for the sqrt(2) distance difference
- `sign(movement)` for direction determination is clean and GPU-friendly

**Verdict: Valid concept, but the mat4 storage is wasteful. The gravity field approach is a legitimate way to model long-range interactions on a grid. Consider mat3 or a packed representation.**

---

### 6. Heat

**Purpose:** Heat diffusion PDE solver.

**What it does:**
- Completely different domain - solves the heat equation: dT/dt = alpha * nabla^2(T) + source
- Cell structure: `{ float temperature, float alpha, float source }`
- Explicit finite difference method with 4-neighbor stencil (no diagonals)
- Physical parameters: thermal conductivity=45.0, specific heat=1.0, density=1.0
- Very small timestep (dt=0.00000001) for numerical stability
- Alpha = conductivity * dt / dx^2
- Ring-shaped heat source at distance 9000-10000 from center
- Visualization: temperature mapped to RGB (red=hot)

**Assessment:**
- The explicit finite difference scheme is the simplest correct approach for heat diffusion
- The 4-neighbor stencil (no diagonals) is standard for the 2D Laplacian
- **Stability concern:** The explicit scheme requires `alpha < 0.25` for stability in 2D (CFL condition). With the given parameters: alpha = 45 * 1e-8 / 1e-4 = 0.0045, which satisfies the condition. Good
- The tiny timestep (1e-8) means the simulation will evolve very slowly per frame. Consider increasing dt while staying below the CFL limit, or switching to an implicit scheme for larger timesteps
- compute2.shader appears incomplete (code terminates mid-line) - this needs to be finished
- Using only 4 neighbors (not 8) is correct for the standard 5-point Laplacian stencil

**Verdict: Valid implementation of explicit finite difference heat equation. Numerically stable with chosen parameters. The incomplete compute2.shader needs attention.**

---

## Overall Architecture Assessment

### What's Working Well

1. **Progressive complexity:** The project shows a clear learning trajectory from basic rendering -> CPU simulation -> GPU compute -> increasingly complex physics. Each branch builds on lessons from the previous one.

2. **Correct GPU compute patterns:** The SSBO ping-pong buffer approach, compute shader dispatch, and two-pass simulation pipeline are all industry-standard techniques for GPU-based grid simulations.

3. **Toroidal wrapping:** Consistently implemented across simulation branches, which is the correct boundary condition for unbounded simulations.

4. **Separation of concerns in compute:** The dual-pass approach (calculate forces, then move mass) avoids race conditions and is physically correct.

### Areas for Improvement

1. **Code organization:** The simulation branches put everything in `Source.cpp`, which grows unwieldy. Consider extracting simulation setup, compute dispatch, and rendering into separate modules even for GPU-based code.

2. **Memory efficiency:** The Directional branch's mat4 for a 3x3 field wastes GPU memory. Profile and optimize cell structures.

3. **Workgroup sizing:** The 25x25 (625 thread) workgroups may not be optimal for all GPUs. Most GPUs prefer workgroup sizes that are multiples of the warp/wavefront size (32 for NVIDIA, 64 for AMD). Consider 16x16 or 32x32.

4. **Heat branch incomplete:** The compute2.shader is cut off mid-line and needs to be completed.

5. **No shared utilities:** Each simulation branch reinvents buffer setup, shader loading, and the render loop. A thin shared utility layer (just for SSBO creation, compute dispatch, and fullscreen quad rendering) would reduce duplication without over-abstracting.

### Is the Approach Valid?

**Yes.** The overall trajectory from CPU-based simulation to GPU compute shaders is exactly the right evolution for grid-based simulations. The specific techniques used (SSBO ping-pong, compute shaders, explicit finite difference for PDEs, directional gravity fields) are all legitimate and well-established in computational physics and GPU computing.

The main architectural risk is that each branch is a standalone experiment with no shared code. If the goal is to eventually combine these techniques (e.g., heat + gravity + particle flow), some refactoring toward a shared compute framework would be needed. But for experimentation and learning, the current branch-per-experiment approach is perfectly reasonable.
