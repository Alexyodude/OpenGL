#include "Debug.h"
#include <cstdarg>
#include <cstdio>
#include <unordered_map>
#include <mutex>
#include <iostream>
#include <chrono>

namespace Debug {

namespace {
    constexpr size_t MAX_LINES = 200;
    std::vector<std::string> g_lines;
    std::mutex g_lines_mutex;

    struct VarEntry { VarType type; void* ptr; };
    std::unordered_map<std::string, VarEntry> g_vars;
    std::unordered_map<std::string, Dumper> g_dumpers;
}

void Log(const char* category, const char* fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    std::string line = "[";
    line += category;
    line += "] ";
    line += buf;

    {
        std::lock_guard<std::mutex> lk(g_lines_mutex);
        g_lines.push_back(line);
        if (g_lines.size() > MAX_LINES) {
            g_lines.erase(g_lines.begin(), g_lines.begin() + (g_lines.size() - MAX_LINES));
        }
    }
    // Also echo to stdout so the external CLI sees it.
    std::cout << line << std::endl;
}

const std::vector<std::string>& GetLines() {
    return g_lines;
}

void ClearLog() {
    std::lock_guard<std::mutex> lk(g_lines_mutex);
    g_lines.clear();
}

void RegisterVar(const std::string& key, double* p) { g_vars[key] = {VarType::Double, p}; }
void RegisterVar(const std::string& key, float* p)  { g_vars[key] = {VarType::Float, p}; }
void RegisterVar(const std::string& key, int* p)    { g_vars[key] = {VarType::Int, p}; }
void RegisterVar(const std::string& key, bool* p)   { g_vars[key] = {VarType::Bool, p}; }

bool SetVar(const std::string& key, const std::string& value, std::string& err) {
    auto it = g_vars.find(key);
    if (it == g_vars.end()) { err = "unknown var: " + key; return false; }
    try {
        switch (it->second.type) {
            case VarType::Double: *static_cast<double*>(it->second.ptr) = std::stod(value); break;
            case VarType::Float:  *static_cast<float*>(it->second.ptr)  = std::stof(value); break;
            case VarType::Int:    *static_cast<int*>(it->second.ptr)    = std::stoi(value); break;
            case VarType::Bool:   *static_cast<bool*>(it->second.ptr)   = (value == "1" || value == "true"); break;
        }
    } catch (const std::exception& e) {
        err = std::string("parse error: ") + e.what();
        return false;
    }
    return true;
}

bool GetVar(const std::string& key, std::string& out) {
    auto it = g_vars.find(key);
    if (it == g_vars.end()) return false;
    char buf[64];
    switch (it->second.type) {
        case VarType::Double: snprintf(buf, sizeof(buf), "%g", *static_cast<double*>(it->second.ptr)); break;
        case VarType::Float:  snprintf(buf, sizeof(buf), "%g", (double)*static_cast<float*>(it->second.ptr)); break;
        case VarType::Int:    snprintf(buf, sizeof(buf), "%d", *static_cast<int*>(it->second.ptr)); break;
        case VarType::Bool:   snprintf(buf, sizeof(buf), "%s", *static_cast<bool*>(it->second.ptr) ? "true" : "false"); break;
    }
    out = buf;
    return true;
}

std::vector<std::string> ListVars() {
    std::vector<std::string> out;
    out.reserve(g_vars.size());
    for (const auto& kv : g_vars) out.push_back(kv.first);
    return out;
}

void RegisterDumper(const std::string& system, Dumper d) { g_dumpers[system] = std::move(d); }

bool Dump(const std::string& system, std::string& out) {
    auto it = g_dumpers.find(system);
    if (it == g_dumpers.end()) return false;
    out = it->second();
    return true;
}

std::vector<std::string> ListDumpers() {
    std::vector<std::string> out;
    out.reserve(g_dumpers.size());
    for (const auto& kv : g_dumpers) out.push_back(kv.first);
    return out;
}

// ---- Counters ----
namespace {
    std::unordered_map<std::string, uint64_t> g_counters;
}

void CounterInc(const char* name, uint64_t by) {
    g_counters[name] += by;
}

std::string CountersDump() {
    if (g_counters.empty()) return "(no counters)";
    std::string out;
    for (const auto& kv : g_counters) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%s=%llu ", kv.first.c_str(), (unsigned long long)kv.second);
        out += buf;
    }
    if (!out.empty() && out.back() == ' ') out.pop_back();
    return out;
}

// ---- Timers ----
namespace {
    std::unordered_map<std::string, double> g_timer_totals_sec;
    std::unordered_map<std::string, uint64_t> g_timer_hits;
}

void TimerAdd(const char* name, double seconds) {
    g_timer_totals_sec[name] += seconds;
    g_timer_hits[name]++;
}

std::string TimersDump() {
    if (g_timer_totals_sec.empty()) return "(no timers)";
    std::string out;
    for (const auto& kv : g_timer_totals_sec) {
        char buf[128];
        uint64_t hits = g_timer_hits[kv.first];
        double avg_ms = hits ? (kv.second * 1000.0 / (double)hits) : 0.0;
        snprintf(buf, sizeof(buf), "%s total=%.3fms hits=%llu avg=%.3fms ",
                 kv.first.c_str(), kv.second * 1000.0, (unsigned long long)hits, avg_ms);
        out += buf;
    }
    if (!out.empty() && out.back() == ' ') out.pop_back();
    return out;
}

ScopeTimer::ScopeTimer(const char* name)
    : m_name(name),
      m_start_ticks((uint64_t)std::chrono::steady_clock::now().time_since_epoch().count()) {}

ScopeTimer::~ScopeTimer() {
    auto end = (uint64_t)std::chrono::steady_clock::now().time_since_epoch().count();
    double dt = double(end - m_start_ticks) * (double)std::chrono::steady_clock::period::num
                                            / (double)std::chrono::steady_clock::period::den;
    TimerAdd(m_name, dt);
}

// ---- Watches ----
namespace {
    enum class WatchKind { Int, Float, Double, Bool };
    struct WatchEntry { WatchKind kind; const void* ptr; };
    // Vector preserves registration order for stable dump output.
    std::vector<std::pair<std::string, WatchEntry>> g_watches;

    void WatchSet(const char* name, WatchKind k, const void* p) {
        for (auto& kv : g_watches) {
            if (kv.first == name) { kv.second = {k, p}; return; }
        }
        g_watches.push_back({name, {k, p}});
    }
}

void WatchAdd(const char* name, const int* p)    { WatchSet(name, WatchKind::Int, p); }
void WatchAdd(const char* name, const float* p)  { WatchSet(name, WatchKind::Float, p); }
void WatchAdd(const char* name, const double* p) { WatchSet(name, WatchKind::Double, p); }
void WatchAdd(const char* name, const bool* p)   { WatchSet(name, WatchKind::Bool, p); }

std::string WatchesDump() {
    if (g_watches.empty()) return "(no watches)";
    std::string out;
    for (const auto& kv : g_watches) {
        char buf[64];
        switch (kv.second.kind) {
            case WatchKind::Int:    snprintf(buf, sizeof(buf), "%s=%d ", kv.first.c_str(), *static_cast<const int*>(kv.second.ptr)); break;
            case WatchKind::Float:  snprintf(buf, sizeof(buf), "%s=%g ", kv.first.c_str(), (double)*static_cast<const float*>(kv.second.ptr)); break;
            case WatchKind::Double: snprintf(buf, sizeof(buf), "%s=%g ", kv.first.c_str(), *static_cast<const double*>(kv.second.ptr)); break;
            case WatchKind::Bool:   snprintf(buf, sizeof(buf), "%s=%s ", kv.first.c_str(), *static_cast<const bool*>(kv.second.ptr) ? "true" : "false"); break;
        }
        out += buf;
    }
    if (!out.empty() && out.back() == ' ') out.pop_back();
    return out;
}

} // namespace Debug
