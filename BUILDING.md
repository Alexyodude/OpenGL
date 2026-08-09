# Building

This branch adds a cross-platform CMake build alongside the original
`OpenGL/OpenGL.vcxproj`. The simulation code is unchanged.

## The GLEW gotcha

GLEW's prebuilt `.lib` files are **not in the repo**. The `.gitignore` inherited
from the Visual Studio template ignores any directory called `x64/`, and GLEW
ships its libraries in `lib/Release/x64/`, so they were silently excluded. A
fresh clone therefore fails to link with:

```
LINK : fatal error LNK1181: cannot open input file
'Dependices\glew-2.1.0\lib\Release\Win32\glew32.lib'
```

Fix: download [glew-2.1.0-win32.zip](https://github.com/nigels-com/glew/releases/tag/glew-2.1.0)
and copy its `lib/` and `bin/` folders into `OpenGL/Dependices/glew-2.1.0/`.
The CI workflow does this automatically.

## Windows

Either the original solution:

```
msbuild OpenGL.sln /p:Configuration=Release /p:Platform=x64
```

(add `/p:PlatformToolset=v145` if you have a newer toolset than the `v143` the
project pins), or CMake:

```
cmake -S . -B build -A x64
cmake --build build --config Release
```

Output: `build/Release/OpenGL.exe`. Uses the GLFW and GLEW libraries vendored in
`OpenGL/Dependices/`.

## macOS

```
brew install glfw glew
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$(brew --prefix)"
cmake --build build --config Release -j
```

Output: `build/OpenGL.app`.

## Linux

```
sudo apt install cmake build-essential libglfw3-dev libglew-dev libgl1-mesa-dev xorg-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Output: `build/OpenGL`.

## CI

`.github/workflows/build.yml` builds all three on every push and uploads:

| Platform | Artifact |
|----------|----------------------------|
| Windows  | `OpenGL-windows-x64.zip`   |
| Linux    | `OpenGL-linux-x64.tar.gz`  |
| macOS    | `OpenGL-macos.dmg`         |

Download them from the run's **Artifacts** section on the Actions tab.

## Portability changes

Four changes were needed to make non-Windows builds behave like the Windows
one. The simulation itself — the wave diffusion, the attraction/repulsion
gradient, momentum transfer, toroidal wrapping, the WASD dipole injection — is
untouched.

**`src/Log.h`: portable debug break.** `ASSERT` was defined as
`if (!(x)) __debugbreak();`, and `__debugbreak()` is an MSVC intrinsic that does
not exist on clang or gcc. Since `GLCall` wraps every GL call in `ASSERT`, this
failed to compile at every call site on macOS and Linux. MSVC keeps the original
intrinsic, so its behaviour is unchanged; clang uses `__builtin_debugtrap()` and
gcc raises `SIGTRAP`.

**`src/Platform.h` (new): deterministic RNG.** The 500x500 grid is seeded from
`rand()` and `srand()` is never called, so the starting world is whatever the C
runtime produces from seed 1. That generator is not standardised — MSVC, glibc
and libc++ each use a different one, and `RAND_MAX` differs (32767 vs
2147483647). Left alone, a Mac build would boot into a different world than the
Windows build. `Platform::Rand()` reproduces MSVC's generator exactly, verified
against `std::rand()` over 250,000 draws with zero mismatches. On Windows it is
a no-op; elsewhere it makes the starting world identical.

**`src/Platform.h` (new): macOS context hints.** macOS only exposes OpenGL past
2.1 through a forward-compatible core profile, requested before window creation.
Without it the `#version 330 core` shaders fail to compile there. Windows and
Linux keep the original default context. GLEW also needs `glewExperimental` on a
core profile.

**`src/SandBox.h`: `vertexCount` initialised to 0.** This was uninitialised. It
seeds every index value in `AddQuad`, so its indeterminate starting value decides
whether the grid renders at all. The Windows/MSBuild build happened to start it
at 0, which is why that build worked; the CMake build started it at garbage and
rendered an empty grey window. Pinned to 0 to lock in the behaviour the working
build already had.

## Known bugs, deliberately left in place

These are preserved so the simulation behaves exactly as it always has:

- **`SandBox.cpp:555`** — the `(-1,-1)` diagonal branch calls
  `SetVerticesMomentum(i, j, ip1, jp1)` where the other three branches use their
  own direction; this one should be `(im1, jm1)`. Collisions to the lower-left
  transfer momentum to the wrong neighbour.
- **`SandBox.cpp:603`** — `OnUpdate(0.05f)` is a hardcoded timestep, so
  simulation speed tracks framerate rather than wall-clock time.
- **`SandBox.cpp:781`** — `SetRenderer` assigns its parameter to itself
  (`m_Renderer = m_Renderer`), so it does nothing. `m_Renderer` stays a null
  `unique_ptr`; `OnRender` calls through it and only works because `Renderer`
  has no data members.
- **`Player.cpp`** — declares members that are shadowed by file-scope globals of
  the same name, and calls `glGenBuffers(2, &player_Buffer)` which writes two
  `GLuint`s into a single `GLuint`. The class is never instantiated, so it is
  inert.
