#include "SimLayer.h"
#include "../core/Shader.h"
#include "../core/Log.h"
#include "GridRenderer.h"
#include <cassert>
#include <GL/glew.h>

void SimLayer::ValidateGridSize() const {
    assert(m_GridWidth % m_WorkgroupSize == 0 &&
        "SimLayer: gridWidth must be divisible by workgroupSize");
    assert(m_GridHeight % m_WorkgroupSize == 0 &&
        "SimLayer: gridHeight must be divisible by workgroupSize");
}

void SimLayer::Step(SSBOManager& ssbo, std::vector<Shader*>& shaders, int nSteps) {
    auto passes = GetPasses();
    if (passes.empty()) return;  // Static layer (e.g. ChemicalsLayer)

    assert(shaders.size() >= passes.size() &&
        "SimLayer::Step: not enough shaders for pass count");

    for (int step = 0; step < nSteps; step++) {
        for (int p = 0; p < (int)passes.size(); p++) {
            const ShaderPass& pass = passes[p];
            Shader* shader = shaders[p];

            shader->Bind();
            SetComputeUniforms(*shader, p);
            ssbo.BindForCompute(pass.readBinding, pass.writeBinding);

            GLCall(glDispatchCompute(
                m_GridWidth / m_WorkgroupSize,
                m_GridHeight / m_WorkgroupSize,
                1));
            GLCall(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));

            if (pass.swapAfter)
                ssbo.SwapBuffers();
        }
    }
}

void SimLayer::Render(SSBOManager& ssbo, Shader& fragmentShader, GridRenderer& renderer,
                      int viewportWidth, int viewportHeight) {
    fragmentShader.Bind();
    fragmentShader.SetUniform1i("u_GridWidth", m_GridWidth);
    fragmentShader.SetUniform1i("u_GridHeight", m_GridHeight);
    fragmentShader.SetUniform1i("u_ViewportWidth", viewportWidth);
    fragmentShader.SetUniform1i("u_ViewportHeight", viewportHeight);
    ssbo.BindForRender();
    renderer.Draw();
}
