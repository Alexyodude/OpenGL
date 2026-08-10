#pragma once
#include <string>
#include <vector>
#include <functional>

// In-app debug console, ported from the GameEng pixel engine.
//
// The command registry and Execute() are the same logic as GameEng's. What
// changed is the presentation layer: GameEng draws the overlay itself with
// SimpleFont and reads SDL_TEXTINPUT events, whereas this build already has
// ImGui, so Render() is an ImGui window instead.
//
// GameEng's console also held an Engine* so `quit` and `screenshot` could reach
// the engine. There is no equivalent type here, so `quit` sets a flag the main
// loop polls, which keeps the console free of any GLFW dependency.

namespace Debug {

class Console {
public:
    // Custom command handler. Args excludes the command name. Output appended to `out`.
    using Handler = std::function<void(const std::vector<std::string>& args, std::string& out)>;

    // Registers built-ins: help, clear, dump, set, get, run, console, quit.
    static void Init();

    static bool IsActive();
    static void Toggle();
    static void SetActive(bool on);

    // Draws the overlay. Call between ImGui::NewFrame() and ImGui::Render().
    static void Render();

    // Run one command line. Used by both the overlay and the external CLI.
    static void Execute(const std::string& line);

    static void Register(const std::string& name, Handler h, const std::string& help);

    // True once `quit` has been run. The main loop polls this.
    static bool QuitRequested();
};

} // namespace Debug
