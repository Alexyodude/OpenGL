#include "DebugCli.h"
#include "DebugConsole.h"
#include "Debug.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <iostream>

namespace Debug {
namespace Cli {

namespace {
    std::atomic<bool> g_running{false};
    std::thread g_thread;
    std::mutex g_q_mutex;
    std::queue<std::string> g_queue;
}

static void ReaderLoop() {
    std::string line;
    while (g_running.load() && std::getline(std::cin, line)) {
        std::lock_guard<std::mutex> lk(g_q_mutex);
        g_queue.push(line);
    }
    // EOF: signal a graceful quit by enqueuing "quit".
    {
        std::lock_guard<std::mutex> lk(g_q_mutex);
        g_queue.push("quit");
    }
    g_running.store(false);
}

void StartStdin() {
    if (g_running.exchange(true)) return;
    ::Debug::Log("CLI", "stdin reader started");
    g_thread = std::thread(ReaderLoop);
    g_thread.detach();
}

void Pump() {
    std::queue<std::string> local;
    {
        std::lock_guard<std::mutex> lk(g_q_mutex);
        std::swap(local, g_queue);
    }
    while (!local.empty()) {
        Console::Execute(local.front());
        local.pop();
    }
}

void Stop() {
    g_running.store(false);
}

} // namespace Cli
} // namespace Debug
