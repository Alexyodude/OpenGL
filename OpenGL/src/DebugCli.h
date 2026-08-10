#pragma once

// External CLI bridge, ported from the GameEng pixel engine.
//
// Opt in at launch with --cli-stdin. Commands are read from stdin on a
// background thread and drained on the main thread once per frame, so they run
// against the same registry the in-app console uses. Anything typeable in the
// console can be scripted:
//
//   OpenGL.exe --cli-stdin < debug/scripts/sandbox.txt
//
// GameEng's version threaded an Engine* through Pump(); there is no equivalent
// type here, so Pump() takes nothing and `quit` is surfaced through
// Console::QuitRequested().

namespace Debug {
namespace Cli {

// Spawns a background thread that reads command lines from stdin and queues them.
void StartStdin();

// Drains queued lines and executes them against the console registry.
// Call once per frame from the main loop. No-op when stdin mode is off.
void Pump();

// Stops the reader thread (best-effort).
void Stop();

} // namespace Cli
} // namespace Debug
