#pragma once

#include <string>

// Describes one compute dispatch pass within a SimLayer's Step() loop.
// The base SimLayer::Step() iterates over GetPasses() and uses these fields
// to drive BindForCompute() and SwapBuffers().
struct ShaderPass {
    std::string shaderPath;  // Path to .comp file
    int readBinding;         // SSBO index to bind as read (binding 0)
    int writeBinding;        // SSBO index to bind as write (binding 1)
    bool swapAfter;          // Call SSBOManager::SwapBuffers() after this pass?
};
