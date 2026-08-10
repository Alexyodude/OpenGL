#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

// Debug instrumentation layer, ported from the GameEng pixel engine.
//
// Nothing here touches OpenGL, GLFW or ImGui - it is plain C++ - so it is the
// same code on both engines. The float overloads are new: GameEng's simulation
// state is double, this one's tunables are all float.
//
// Every macro compiles to a no-op unless DEBUG_BUILD is defined, so release
// builds carry zero overhead and the simulation is bit-identical to before.

namespace Debug {

// Log ring buffer (capacity = 200 lines).
void Log(const char* category, const char* fmt, ...);
const std::vector<std::string>& GetLines();
void ClearLog();

// Variable registry for `set <key> <value>` / `get <key>`.
enum class VarType { Double, Float, Int, Bool };
void RegisterVar(const std::string& key, double* p);
void RegisterVar(const std::string& key, float* p);
void RegisterVar(const std::string& key, int* p);
void RegisterVar(const std::string& key, bool* p);
bool SetVar(const std::string& key, const std::string& value, std::string& err);
bool GetVar(const std::string& key, std::string& out);
std::vector<std::string> ListVars();

// Dumper registry for `dump <system>`.
using Dumper = std::function<std::string()>;
void RegisterDumper(const std::string& system, Dumper d);
bool Dump(const std::string& system, std::string& out);
std::vector<std::string> ListDumpers();

// Counter registry (lifetime totals).
void CounterInc(const char* name, uint64_t by = 1);
std::string CountersDump();

// Timer registry (RAII scope accumulators).
void TimerAdd(const char* name, double seconds);
std::string TimersDump();

class ScopeTimer {
public:
    ScopeTimer(const char* name);
    ~ScopeTimer();
private:
    const char* m_name;
    uint64_t m_start_ticks;
};

// Watch registry - type-erased pointers stringified on demand.
void WatchAdd(const char* name, const int* p);
void WatchAdd(const char* name, const float* p);
void WatchAdd(const char* name, const double* p);
void WatchAdd(const char* name, const bool* p);
std::string WatchesDump();

} // namespace Debug

#ifdef DEBUG_BUILD
    #define DEBUG_LOG(cat, ...) ::Debug::Log(cat, __VA_ARGS__)
    #define DEBUG_ASSERT(cond, msg) do { if (!(cond)) { ::Debug::Log("ASSERT", "%s:%d %s", __FILE__, __LINE__, msg); } } while (0)
    #define DEBUG_COUNTER(name) ::Debug::CounterInc(name)
    #define DEBUG_TIMER_CONCAT_(a,b) a##b
    #define DEBUG_TIMER_CONCAT(a,b) DEBUG_TIMER_CONCAT_(a,b)
    #define DEBUG_TIMER(name) ::Debug::ScopeTimer DEBUG_TIMER_CONCAT(_dbg_timer_, __LINE__)(name)
    #define DEBUG_WATCH(name, ptr) ::Debug::WatchAdd(name, ptr)
#else
    #define DEBUG_LOG(cat, ...) ((void)0)
    #define DEBUG_ASSERT(cond, msg) ((void)0)
    #define DEBUG_COUNTER(name) ((void)0)
    #define DEBUG_TIMER(name) ((void)0)
    #define DEBUG_WATCH(name, ptr) ((void)0)
#endif
