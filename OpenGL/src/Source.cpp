#define _compute2

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>
#include "Shader.h"
#include <chrono>

const int width = 1000;
const int height = 1000;
const int gridWidth = 1000;
const int gridHeight = 1000;

struct Cell {
    glm::vec2 velocity;//2
    glm::vec2 movement;//4
    glm::mat4 gravity;//13
    //glm::vec4 b;
    //glm::vec4 a;
    float a;
    float mass;//14
    float repulse;//15
    float extra;//16
};

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(width, height, "Compute Shader Example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewInit();

    // Create shader program for compute shader
    std::unique_ptr<Shader> computeProgram = std::make_unique<Shader>("res/shaders/compute.shader", GL_COMPUTE_SHADER);

#ifdef _compute2
    // Create shader program for the second compute shader
    std::unique_ptr<Shader> computeProgram2 = std::make_unique<Shader>("res/shaders/compute2.shader", GL_COMPUTE_SHADER);
#endif
    // Create shader program for rendering
    std::unique_ptr<Shader> renderProgram = std::make_unique<Shader>("res/shaders/vertex.shader", "res/shaders/fragment.shader");

    // Initialize grid with random state
    std::vector<Cell> grid1(gridWidth * gridHeight);
    std::vector<Cell> grid2(gridWidth * gridHeight);

    for (int i = 0; i < gridWidth * gridHeight; i++) {
        grid1[i].mass = (rand() % 1000 == 0) ? 1.0f : 0.0f;
        
        grid1[i].gravity = glm::mat3(
              { grid1[i].mass * 0.125 , grid1[i].mass * 0.125   , grid1[i].mass * 0.125 }
            , { grid1[i].mass * 0.125 , grid1[i].mass           , grid1[i].mass * 0.125 }
            , { grid1[i].mass * 0.125 , grid1[i].mass * 0.125   , grid1[i].mass * 0.125 });
        grid1[i].velocity = glm::vec2((rand() % 500 - 250) * 0.01, (rand() % 500 - 250) * 0.01);
        //grid1[i].velocity = glm::vec2(0.0f);
        grid1[i].movement = glm::vec2(0.0f);
        grid1[i].repulse = grid1[i].mass;
        //grid1[i].test = glm::vec2(grid1[i].mass-0.5);
    }

    std::cout << sizeof(Cell) << std::endl;

    // Create SSBOs
    GLuint ssbos[2];
    glGenBuffers(2, ssbos);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbos[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, gridWidth * gridHeight * sizeof(Cell), grid1.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbos[0]);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbos[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, gridWidth * gridHeight * sizeof(Cell), grid2.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbos[1]);


    int currentSSBOIndex = 0;

    auto lastTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        float milliseconds = 0.0f;
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsedTime = currentTime - lastTime;
        milliseconds += elapsedTime.count() * 1000.0f;
        frameCount++;

        if (milliseconds >= 1000.0f) {
            float fps = frameCount / (milliseconds / 1000.0f);
            std::string title = "Compute Shader Example - FPS: " + std::to_string(fps) + " - " + std::to_string(milliseconds / frameCount) + " ms/frame";
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            milliseconds = 0.0f;
            lastTime = currentTime;
        }

        // 1. Run the first compute shader
        computeProgram->Bind();

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbos[0]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbos[1]);

        glDispatchCompute(gridWidth / 25, gridHeight / 25, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // 2. Run the second compute shader
        //computeProgram->Unbind();
#ifdef _compute2
        computeProgram2->Bind();

        // You may need to adjust the buffer bindings depending on your data flow
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbos[1]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbos[0]);

        glDispatchCompute(gridWidth / 25, gridHeight / 25, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
#endif
        // 3. Render the output
        renderProgram->Bind();

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // 4. Swap SSBO indices for next frame
#ifndef _compute2
        currentSSBOIndex = (currentSSBOIndex + 1) % 2;
#endif // _compute2

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
