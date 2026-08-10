#include "DebugConsole.h"
#include "Debug.h"

#include "imgui/imgui.h"

#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <cstring>

namespace Debug {

namespace {
    bool g_active = false;
    bool g_quit_requested = false;

    std::vector<std::string> g_history;
    int g_history_idx = -1; // -1 = current (uncommitted) input

    struct Cmd { Console::Handler fn; std::string help; };
    std::unordered_map<std::string, Cmd> g_cmds;

    char g_input[512] = {0};
    bool g_scroll_to_bottom = true;

    std::vector<std::string> Split(const std::string& s) {
        std::vector<std::string> out;
        std::istringstream iss(s);
        std::string tok;
        while (iss >> tok) out.push_back(tok);
        return out;
    }

    std::vector<std::string> SortedCommandNames() {
        std::vector<std::string> names;
        names.reserve(g_cmds.size());
        for (auto& kv : g_cmds) names.push_back(kv.first);
        std::sort(names.begin(), names.end());
        return names;
    }
}

void Console::Init() {
    Register("help", [](const std::vector<std::string>&, std::string& out) {
        out += "Commands:\n";
        for (auto& n : SortedCommandNames()) {
            out += "  "; out += n; out += " - "; out += g_cmds[n].help; out += "\n";
        }
    }, "list commands");

    Register("clear", [](const std::vector<std::string>&, std::string&) {
        ::Debug::ClearLog();
    }, "clear log");

    Register("quit", [](const std::vector<std::string>&, std::string&) {
        // Handled in Execute so it works identically from the CLI.
    }, "exit the app");

    Register("dump", [](const std::vector<std::string>& args, std::string& out) {
        if (args.empty()) {
            out += "systems:";
            for (auto& s : ListDumpers()) { out += " "; out += s; }
            out += "\n";
            return;
        }
        std::string d;
        if (Dump(args[0], d)) { out += d; if (!d.empty() && d.back() != '\n') out += "\n"; }
        else { out += "unknown system: "; out += args[0]; out += "\n"; }
    }, "dump <system>");

    Register("set", [](const std::vector<std::string>& args, std::string& out) {
        if (args.size() < 2) { out += "usage: set <key> <value>\n"; return; }
        std::string err;
        if (!SetVar(args[0], args[1], err)) { out += err; out += "\n"; }
        else { out += "ok\n"; }
    }, "set <key> <value>");

    Register("get", [](const std::vector<std::string>& args, std::string& out) {
        if (args.empty()) {
            out += "vars:";
            for (auto& v : ListVars()) { out += " "; out += v; }
            out += "\n";
            return;
        }
        std::string v;
        if (GetVar(args[0], v)) { out += args[0]; out += " = "; out += v; out += "\n"; }
        else { out += "unknown var: "; out += args[0]; out += "\n"; }
    }, "get [<key>]");

    Register("console", [](const std::vector<std::string>& args, std::string& out) {
        if (args.empty()) { out += g_active ? "on\n" : "off\n"; return; }
        if (args[0] == "on")          SetActive(true);
        else if (args[0] == "off")    SetActive(false);
        else if (args[0] == "toggle") Toggle();
        else { out += "usage: console [on|off|toggle]\n"; return; }
        out += "ok\n";
    }, "console [on|off|toggle]");

    Register("run", [](const std::vector<std::string>& args, std::string& out) {
        if (args.empty()) { out += "usage: run <scriptfile>\n"; return; }
        std::ifstream f(args[0]);
        if (!f) { out += "cannot open: "; out += args[0]; out += "\n"; return; }
        std::string line;
        while (std::getline(f, line)) Execute(line);
        out += "ran "; out += args[0]; out += "\n";
    }, "run <scriptfile>");

    // Built-in dumpers for the instrumentation primitives.
    RegisterDumper("counters", []() { return CountersDump(); });
    RegisterDumper("timers",   []() { return TimersDump(); });
    RegisterDumper("watches",  []() { return WatchesDump(); });
    RegisterDumper("log", []() {
        const auto& lines = GetLines();
        size_t n = lines.size();
        size_t start = n > 20 ? n - 20 : 0;
        std::string out;
        for (size_t i = start; i < n; ++i) { out += lines[i]; out += "\n"; }
        return out;
    });
}

bool Console::IsActive() { return g_active; }
bool Console::QuitRequested() { return g_quit_requested; }

void Console::Toggle() { SetActive(!g_active); }

void Console::SetActive(bool on) {
    if (on == g_active) return;
    g_active = on;
    if (!g_active) {
        g_input[0] = '\0';
        g_history_idx = -1;
    }
}

void Console::Register(const std::string& name, Handler h, const std::string& help) {
    g_cmds[name] = {std::move(h), help};
}

void Console::Execute(const std::string& raw) {
    std::string line = raw;
    while (!line.empty() && (line.back() == '\n' || line.back() == '\r' || line.back() == ' ')) line.pop_back();
    // Strip UTF-8 BOM if present, plus any leading whitespace/control bytes.
    if (line.size() >= 3 &&
        (unsigned char)line[0] == 0xEF && (unsigned char)line[1] == 0xBB && (unsigned char)line[2] == 0xBF) {
        line.erase(0, 3);
    }
    while (!line.empty() && ((unsigned char)line.front() <= ' ')) line.erase(0, 1);
    if (line.empty()) return;
    if (line[0] == '#') return; // script comment

    ::Debug::Log("CONSOLE", "> %s", line.c_str());
    auto toks = Split(line);
    if (toks.empty()) return;

    if (toks[0] == "quit") {
        g_quit_requested = true;
        return;
    }

    auto it = g_cmds.find(toks[0]);
    if (it == g_cmds.end()) {
        ::Debug::Log("CONSOLE", "unknown command: %s", toks[0].c_str());
        return;
    }
    std::vector<std::string> args(toks.begin() + 1, toks.end());
    std::string out;
    it->second.fn(args, out);

    // Split multi-line output into separate log entries.
    if (!out.empty()) {
        std::string cur;
        for (char c : out) {
            if (c == '\n') {
                if (!cur.empty()) { ::Debug::Log("CONSOLE", "%s", cur.c_str()); cur.clear(); }
            } else {
                cur += c;
            }
        }
        if (!cur.empty()) ::Debug::Log("CONSOLE", "%s", cur.c_str());
    }
    g_scroll_to_bottom = true;
}

namespace {
// Drives Up/Down history and Tab completion on the input box.
int InputCallback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory) {
        const int prev = g_history_idx;
        if (data->EventKey == ImGuiKey_UpArrow) {
            if (g_history_idx == -1) g_history_idx = (int)g_history.size() - 1;
            else if (g_history_idx > 0) g_history_idx--;
        } else if (data->EventKey == ImGuiKey_DownArrow) {
            if (g_history_idx != -1 && ++g_history_idx >= (int)g_history.size()) g_history_idx = -1;
        }
        if (prev != g_history_idx) {
            const char* s = (g_history_idx >= 0) ? g_history[g_history_idx].c_str() : "";
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, s);
        }
    } else if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion) {
        // Complete on the first word only.
        std::string cur(data->Buf, data->Buf + data->BufTextLen);
        if (cur.find(' ') != std::string::npos) return 0;
        std::vector<std::string> hits;
        for (auto& n : SortedCommandNames()) {
            if (n.compare(0, cur.size(), cur) == 0) hits.push_back(n);
        }
        if (hits.size() == 1) {
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, hits[0].c_str());
            data->InsertChars(data->CursorPos, " ");
        } else if (hits.size() > 1) {
            std::string out = "candidates:";
            for (auto& h : hits) { out += " "; out += h; }
            ::Debug::Log("CONSOLE", "%s", out.c_str());
            g_scroll_to_bottom = true;
        }
    }
    return 0;
}
} // namespace

void Console::Render() {
    if (!g_active) return;

    const ImGuiIO& io = ImGui::GetIO();
    const float h = io.DisplaySize.y * 0.40f; // bottom 40%, same proportion as GameEng
    ImGui::SetNextWindowPos(ImVec2(0.0f, io.DisplaySize.y - h), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, h), ImGuiCond_Always);

    if (!ImGui::Begin("Debug Console", nullptr,
                      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    const float footer = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("scrollback", ImVec2(0, -footer), false, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& line : ::Debug::GetLines()) ImGui::TextUnformatted(line.c_str());
    if (g_scroll_to_bottom) { ImGui::SetScrollHereY(1.0f); g_scroll_to_bottom = false; }
    ImGui::EndChild();

    ImGui::Separator();
    ImGui::SetNextItemWidth(-1.0f);
    const ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue
                                    | ImGuiInputTextFlags_CallbackHistory
                                    | ImGuiInputTextFlags_CallbackCompletion;
    if (ImGui::InputText("##cmd", g_input, sizeof(g_input), flags, InputCallback)) {
        std::string line = g_input;
        if (!line.empty()) {
            g_history.push_back(line);
            g_history_idx = -1;
            Execute(line);
        }
        g_input[0] = '\0';
        ImGui::SetKeyboardFocusHere(-1);
    }
    if (ImGui::IsWindowAppearing()) ImGui::SetKeyboardFocusHere(-1);

    ImGui::End();
}

} // namespace Debug
