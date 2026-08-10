# OMC Project Rules — OpenGL Mass/Wave Sandbox

Authoritative for this repository. Unlike the GameEng copy, this file is **not**
a mirror of `.claude/CLAUDE.md` — that file does not exist here, so nothing
overrides this one.

Agents launched via OMC (`autopilot`, `ralph`, `ultrawork`, `team`, `executor`,
`planner`, etc.) MUST read this before touching `OpenGL/src/` or
`OpenGL/res/shaders/`.

This describes the `cross-platform-build` branch, the only branch carrying the
debug surface, the CMake build and CI.

---

## Hard constraints

1. **The simulation's behaviour is frozen on purpose.** These are known bugs,
   deliberately preserved so the build matches the released one. Do not "fix"
   them without being asked:
   - `SandBox.cpp:564` — the `(-1,-1)` diagonal branch tests `im1 + jm1` for
     emptiness but then transfers momentum to `(ip1, jp1)`; should be
     `(im1, jm1)`. Note line 534 looks identical and is *correct* — that is the
     `(1,1)` branch.
   - `SandBox.cpp:612` — hardcoded `OnUpdate(0.05f)`; sim speed tracks framerate.
   - `SandBox.cpp:791` — `SetRenderer` assigns its parameter to itself.

   Line numbers are as of this branch. Grep the expression rather than trusting
   them if the file has moved since.

   Two exceptions already made, both required for the build to work at all:
   `vertexCount` is initialised to 0 (indeterminate, decided whether anything
   rendered), and `Platform::Rand()` replaces `std::rand()` so every platform
   seeds the same world.

2. **Release builds must stay behaviourally identical.** The debug surface is
   behind `DEBUG_BUILD`, OFF by default. In release every macro compiles to a
   no-op, the console cannot open, and `--cli-stdin` is refused with a warning.
   Anything that changes release behaviour is a rule violation.

3. **Extend the debug surface with each new system.** Same three layers as
   GameEng, different paths:
   - L1 `OpenGL/src/Debug.h` — `DEBUG_LOG`, `DEBUG_ASSERT`, `DEBUG_COUNTER`,
     `DEBUG_TIMER`, `DEBUG_WATCH`
   - L2 `OpenGL/src/DebugConsole.h` — one `Debug::Console::Register` for
     `dump <system>`, plus `Debug::RegisterVar` for every new tunable. Overlay is
     ImGui, toggled with `` ` ``.
   - L3 `OpenGL/src/DebugCli.h` — free once L2 exists; the registry is shared.
   - Add a script under `debug/scripts/` exercising the new path.

4. **Verify both build modes before claiming completion**, and quote real
   output. `-DDEBUG_BUILD=OFF` and `-DDEBUG_BUILD=ON`.

5. **Author and verifier are separate passes.** The agent that wrote the code
   does not approve it. Route to `code-reviewer` or `verifier`.

---

## Current debug surface

| Command | |
|---|---|
| `help` | list commands |
| `get [<key>]` | list vars, or read one |
| `set <key> <value>` | write a var live |
| `dump [<system>]` | list dumpers, or run one |
| `run <scriptfile>` | execute a script |
| `console [on\|off\|toggle]` | control the overlay |
| `clear` / `quit` | |

Dumpers: `sandbox`, `counters`, `timers`, `watches`, `log`.
Vars: `well`, `mDispersion`, `repulse`, `dispersion`, `intensity`, `accelDamp`.

```
cmake -S . -B build-dbg -A x64 -DDEBUG_BUILD=ON
cmake --build build-dbg --config Release
build-dbg\Release\OpenGL.exe --cli-stdin < debug\scripts\sandbox.txt
```

EOF on stdin enqueues `quit`, so the app exits when the script is consumed.

---

## Traps in this repo — read before debugging a "missing file"

**The inherited Visual Studio `.gitignore` has eaten required files twice.**
Both failed only on a fresh clone, never for the author.

- `x64/` swallowed `OpenGL/Dependices/glew-2.1.0/lib/Release/x64/`, so
  `find_package`/link fails until GLEW 2.1.0 is downloaded manually.
- `[Dd]ebug/` swallowed the repo-root `debug/` directory holding console
  scripts. Fixed with an explicit `!/debug/`.

If a file exists locally but a clean checkout cannot find it, check
`git check-ignore -v <path>` first.

**Absolute paths.** The `Heat` branch's vcxproj listed
`D:\VS\OpenGL\OpenGL\Dependices\Imgui`, so it built only on the original
machine. Use `$(SolutionDir)`.

**Catch MSVC-isms without a Mac or Linux box.** This code met only MSVC for
years and accumulated five constructs clang and gcc reject. Syntax-check with
g++ against the vendored headers before pushing — no packages, no linking:

```bash
cd OpenGL
INC="-Isrc -Isrc/vendor -IDependices/GLFW/include -IDependices/glew-2.1.0/include"
for f in src/*.cpp src/tests/*.cpp; do
    g++ -fsyntax-only -std=c++17 -DGLEW_STATIC -DGLEW_NO_GLU $INC "$f" || echo "FAILED: $f"
done
```

---

## Platform limits

- **The four GPU compute branches cannot run on macOS.** Compute shaders need
  OpenGL 4.3; Apple caps at 4.1. Only the CPU sim on this branch is
  three-platform. CI must express this per-app rather than running all three
  everywhere.
- CI is `.github/workflows/build.yml`, producing a `.zip`, `.tar.gz` and `.dmg`.

---

## Branch reality

`master` has not moved since 2023-05-30 and is an ancestor of every branch;
nothing is behind it. `Yay` is byte-identical to master. The GPU branches
(`Working-Two-Memory-Buffer`, `Changing-Attraction-to-Directional`, `Heat`) are
**siblings** one commit past `5e0cf83`, not a chain. See `BRANCH_ANALYSIS.md` on
the `Claude` branch, and treat its inferred claims with suspicion — its
file-level observations held up, its structural inferences did not.

---

## Delegation guidance

| Work | Route to |
|---|---|
| New sim system + debug wiring | `executor` (opus for non-trivial) |
| Engine/app separation | `architect` → `planner` → `executor` |
| GLSL / compute shader uncertainty | `document-specialist` |
| Post-implementation review | `code-reviewer` then `verifier` |
| Tracing a rendering regression | `tracer` / `debugger` |
