# Engine / Application Separation

**Date:** 2026-08-09
**Status:** Approved, not yet implemented
**Scope:** Split `Alexyodude/OpenGL` into a reusable engine and the simulations built on it.

---

## Motivation

`Alexyodude/OpenGL` holds two things that have never been distinguished: a small
OpenGL framework, and five separate simulation experiments built on top of it.
Because there is no boundary, each experiment forked the whole repository and
diverged. Nothing has ever merged back, and `master` has not moved since
2023-05-30.

The cost is visible: shader loading, window setup and the SSBO ping-pong loop
are copy-pasted across four branches, and improvements made in one are invisible
to the rest.

---

## Evidence

### Branch topology

Verified with `git merge-base` rather than taken from the existing
`BRANCH_ANALYSIS.md`, which is wrong on all three points below.

```
master (2023-05-30)  ==  Yay          ancestor of every branch
│
├── branch-practice (2023-12-21)      +4   CPU mass/wave sim
│   └── cross-platform-build          +9   CMake, CI, portability fixes
│
├── Two-Texture-Method-4x4 (2024-08-17)  +2   GPU rewrite
│   └── 5e0cf83 "optimizing" (2024-08-27)
│       ├── Working-Two-Memory-Buffer    +1
│       ├── Changing-Attraction-to-Directional  +1
│       └── Heat (2025-03-24)            +1
│
└── Claude (2026-04-08)               +1   BRANCH_ANALYSIS.md only
```

Corrections to `BRANCH_ANALYSIS.md`:

1. The GPU branches are **siblings**, not a chain. `Working-Two-Memory-Buffer`
   is not an ancestor of `Heat` or `Changing-Attraction`; all three fan out from
   `5e0cf83` and are exactly one commit each.
2. `Yay` is byte-identical to `master` (0 ahead, 0 behind).
3. Every branch is 0 behind master, because master never advanced.

### Which files are actually engine

Assigned by what survived independent evolution, not by intuition. The GPU
branches deleted the CPU rendering abstractions outright.

| Present in every branch | Deleted by the GPU line |
|---|---|
| `Log.h/.cpp` (`GLCall`) | `Renderer` |
| `Shader.h/.cpp` | `VertexArray`, `VertexBuffer`, `IndexBuffer` |
| `EventListener.h` | `VertexBufferLayout` |
| GLFW/GLEW bootstrap in `Source.cpp` | `Texture`, `SandBox`, `Layer`, `Player` |

The left column is the engine core. The right column is not "not engine" — it is
one rendering *strategy*, which the engine will offer as `render/` alongside a
`compute/` counterpart.

### `Shader` is a superset on the GPU line

| Branch | Constructors |
|---|---|
| `master` | filepath only |
| `branch-practice` | inline vertex+fragment only — **lost** filepath loading |
| `Heat`, `Working-Two-Memory` | filepath, inline vertex+fragment, single-shader-with-type, plus `SetUniform2f` |

The engine inherits `Shader` from the GPU line. `branch-practice` is a
narrowing, not an advance. Porting the CPU sim to the superset is
`const char*` → `const std::string&`, source-compatible at every call site.

### The reusable compute pattern

`Heat/Source.cpp` is 171 lines and contains exactly one reusable pattern: two
SSBOs bound to binding points 0 and 1, swapped each frame, `glDispatchCompute`
followed by `glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT)`, then a
`GL_TRIANGLE_STRIP` fullscreen quad. That pattern is duplicated across four
branches and becomes `compute/`.

---

## Decisions

### Repositories

| Repo | Contents |
|---|---|
| `Alexyodude/PixelEngine` (new) | engine only; builds `pe::engine` static library |
| `Alexyodude/FieldSim` (new) | the five experiments as applications |
| `Alexyodude/OpenGL` | archived read-only; history preserved |

Repo names are provisional.

### Engine scope

Serves **both** rendering strategies. `core/` is strategy-neutral; `render/` and
`compute/` are parallel, independently usable layers. An application links the
engine and uses whichever it needs; neither pulls the other in.

### Layout

```
PixelEngine/
├── include/pe/
│   ├── core/     Window  Input  Shader  Log  Platform
│   ├── render/   Renderer  VertexArray  VertexBuffer
│   │             IndexBuffer  VertexBufferLayout  Texture
│   └── compute/  StorageBuffer  ComputeProgram  FullscreenQuad
├── src/          mirrors include/
├── examples/     triangle/  compute-smoke/
├── vendor/       GLFW  GLEW  glm  stb_image  imgui
└── CMakeLists.txt

FieldSim/
├── apps/
│   ├── mass-wave/     from branch-practice     (CPU, render/)
│   ├── game-of-life/  from Two-Texture         (GPU, compute/)
│   ├── mass-flow/     from Working-Two-Memory  (GPU, compute/)
│   ├── gravity/       from Changing-Attraction (GPU, compute/)
│   └── heat/          from Heat                (GPU, compute/)
└── CMakeLists.txt
```

### Consumption

`FieldSim` pulls the engine with CMake `FetchContent` pinned to a tag
(`GIT_TAG v0.1.0`), not a git submodule. Submodules require a detached-HEAD
update dance on every engine change; `FetchContent` gives equivalent
reproducibility with no extra clone step. This matters because the engine will
change *because* an app needs it, so that path must stay cheap.

### Carried forward from `cross-platform-build`

Archiving `OpenGL` freezes work that must not be lost. These move into
`PixelEngine/core/Platform`:

- `Platform::Rand()` — MSVC's generator reproduced exactly, verified against
  `std::rand()` over 250,000 draws with zero mismatches. Without it `mass-wave`
  boots a different world on macOS and Linux than on Windows.
- macOS forward-compatible core profile hints and `glewExperimental`.
- Portable `DEBUG_BREAK()` replacing the MSVC-only `__debugbreak()`.
- `VertexBufferLayout` specialisations at namespace scope.
- `#include <memory>` where `std::unique_ptr` is used.

The CMake build and the three-platform CI workflow also move to both new repos.

### Bug policy

**Simulation bugs are preserved.** These stay in `apps/mass-wave` so it behaves
exactly as the existing build does:

- `SandBox.cpp:555` — the `(-1,-1)` diagonal branch transfers momentum to
  `(ip1, jp1)` instead of `(im1, jm1)`.
- `SandBox.cpp:603` — hardcoded `OnUpdate(0.05f)` timestep.
- `SandBox.cpp:781` — `SetRenderer` assigns its parameter to itself.

**Engine bugs are fixed**, because they cannot survive extraction:

- `Renderer.h`'s `static int Window_Width/Height` has internal linkage, giving
  every translation unit its own copy. The resize callback updates
  `Source.cpp`'s copy while `SandBox`'s ortho projection reads a stale one.
  Becomes `pe::Window::Width()` / `Height()`.
- `SandBox::m_Renderer` is a null `unique_ptr` that is dereferenced, and only
  works because `Renderer` has no data members. Undefined behaviour.

This changes observable behaviour on window resize. Accepted.

`Player.cpp` is not ported. It is never instantiated, its members are shadowed
by file-scope globals, and `glGenBuffers(2, &player_Buffer)` writes two `GLuint`s
into one. It stays in the archived repo.

---

## Migration order

This is two implementation cycles, not one. **Steps 1–3 (build the engine) and
steps 4–7 (port the apps) should each get their own plan.** Step 3's API is
designed from a single reference branch, so committing to app-porting details
before `compute/` has a real consumer would be planning on assumptions.

Each step ends with a working build; no step depends on a later one.

1. **Create `PixelEngine`** with `core/` only, plus the `triangle` example.
   Carry over the portability fixes and the CI workflow. Tag `v0.1.0`.
2. **Add `render/`** — the CPU-quad classes from `branch-practice`.
3. **Add `compute/`** — `StorageBuffer`, `ComputeProgram`, `FullscreenQuad`,
   extracted from `Heat/Source.cpp`. Add the `compute-smoke` example.
4. **Create `FieldSim`, port `mass-wave` first.** This is the strongest
   verification available: its output can be compared against the known-good
   build.
5. **Port `heat` second.** First real consumer of `compute/`; proves the engine
   serves both strategies before three more apps depend on it.
6. **Port `game-of-life`, `mass-flow`, `gravity`.**
7. **Archive `Alexyodude/OpenGL`.** Only after every app is ported and building.

---

## Verification

- **`mass-wave` must render identically to the current build.** Reference
  screenshots exist from the restored Windows build: green mass self-organised
  into filaments, white `tanh` wave-field glow, grey background outside the
  151×151 window, orange player cell. Same seed, same starting world, because
  `Platform::Rand()` is deterministic across platforms.
- **Both build systems stay green on Windows** — MSBuild and CMake.
- **CI matrix is per-app, not uniform.** The engine and `mass-wave` build on
  Windows, macOS and Linux. The four GPU apps build on Windows and Linux only,
  for the OpenGL 4.3 reason below. The workflow must express this rather than
  running all three everywhere and failing.
- **`g++ -fsyntax-only` against vendored headers** before pushing, to catch
  MSVC-only constructs locally instead of one CI round trip at a time. Five such
  bugs were found this way after CI had been surfacing them one build per fix.

---

## Risks

- **`compute/` is designed from one branch's usage.** `Heat` is the reference;
  the other three GPU branches may need shapes it does not cover. Step 5 exists
  to surface that before three more apps commit to the API.
- **The GPU apps have never been built on macOS or Linux.** They are known to
  compile only under MSVC, and the CPU sim alone needed five portability fixes.
  Expect similar in each. Compute shaders also require OpenGL 4.3, which macOS
  does not support at all — **the four GPU apps will be Windows/Linux only.**
  This is a hard platform limit, not a porting gap.
- **Repo names are provisional** and cheap to change before creation, expensive
  after.

---

## Open questions

None blocking. Naming can be settled at creation time.
