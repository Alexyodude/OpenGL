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
    float mass;
    float state2;
};

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(width, height, "Shader SSBO Example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewInit();

    std::unique_ptr<Shader> program = std::make_unique<Shader>("res/shaders/vertex.shader", "res/shaders/fragment.shader");
    program->Bind();

    std::vector<Cell> grid1(gridWidth * gridHeight);
    std::vector<Cell> grid2(gridWidth * gridHeight);

    for (int i = 0; i < gridWidth * gridHeight; i++) {
        grid1[i].mass = (rand() % 100 == 0) ? 0.9f : 0.0f;
        grid1[i].state2 = grid1[i].mass;
    }

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
            std::string title = "Ya - FPS: " + std::to_string(fps) + " - " + std::to_string(milliseconds / frameCount) + " ms/frame";
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            milliseconds = 0.0f;
            lastTime = currentTime;
        }

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbos[currentSSBOIndex]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbos[(currentSSBOIndex + 1) % 2]);

        glClear(GL_COLOR_BUFFER_BIT);

        glViewport(0, 0, width, height);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        currentSSBOIndex = (currentSSBOIndex + 1) % 2;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
