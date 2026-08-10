//#define _compute2
#include <windows.h>
extern "C" {
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>
#include "Shader.h"
#include <chrono>
#include "imgui.h"
//#include "imgui_impl_glfw.h"
//#include "imgui_impl_opengl3.h"

const int width = 1000;
const int height = 1000;
const int gridWidth = 1000;
const int gridHeight = 1000;

struct Cell {
	float temperature;
    float alpha;
    float source;
};

float func(int i, int j) {
    return ((i-gridWidth/2)* (i - gridWidth/2) + (j-gridHeight/2)* (j - gridHeight/2));
	//return (i + j - 100);
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(width, height, "Compute Shader Example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
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

    
    float scale = 0.1f;
	//float gridScale = scale/gridWidth;
	float thermalConductivity = 45.0f;
	float specificHeat = 1.0f;
	float density = 1.0f;
	float deltaTime = 0.00000001f;
	float deltaSpace = 0.01f;

    for (int i = 0; i < gridWidth; i++) {
		for (int j = 0; j < gridHeight; j++) {
            if (i == 0 || j == 0 || i == gridWidth || j == gridHeight) {
				grid1[i * gridWidth + j].temperature = 0.0f;
				grid1[i * gridWidth + j].alpha = 0.0f;
				grid1[i * gridWidth + j].source = 0.0f;
            }
            //else if (func(i,j) > 50) {
			else if (func(i,j) < 10000 && func(i, j) > 9000) {
				//std::cout << "i: " << i << " j: " << j << std::endl;
				grid1[i * gridWidth + j].temperature = 100.0f;
				grid1[i * gridWidth + j].alpha = thermalConductivity * deltaTime/(deltaSpace*deltaSpace);
				grid1[i * gridWidth + j].source = 0.15f;
				//std::cout << "i: " << i << " j: " << j << " " << func(i, j) << std::endl;
                
            }
            else {
                grid1[i * gridWidth + j].temperature = 0.0f;
                grid1[i * gridWidth + j].alpha = thermalConductivity * deltaTime / (deltaSpace * deltaSpace);
				grid1[i * gridWidth + j].source = 0.0f;
            }
			
		}
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

	float time = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float milliseconds = 0.0f;
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsedTime = currentTime - lastTime;
        milliseconds += elapsedTime.count() * 1000.0f;
        frameCount++;

        if (milliseconds >= 1000.0f) {
            float fps = frameCount / (milliseconds / 1000.0f);
            time = time + frameCount * deltaTime;
            std::string title = std::to_string(time) + "s - " + "Compute Shader Example - FPS: " + std::to_string(fps) + " - " + std::to_string(milliseconds / frameCount) + " ms/frame";
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            milliseconds = 0.0f;
            lastTime = currentTime;
        }

        // 1. Run the first compute shader
        computeProgram->Bind();

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbos[currentSSBOIndex]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbos[(currentSSBOIndex + 1) % 2]);

        glDispatchCompute(gridWidth / 25, gridHeight / 25, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // 2. Run the second compute shader
        //computeProgram->Unbind();
#ifdef _compute2
        computeProgram2->Bind();

        // Read what pass 1 just wrote and write back into the buffer it read
        // from. These were hardcoded to ssbos[1] and ssbos[0], which ignored
        // currentSSBOIndex and so read the wrong buffer on every other frame.
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbos[(currentSSBOIndex + 1) % 2]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbos[currentSSBOIndex]);

        glDispatchCompute(gridWidth / 25, gridHeight / 25, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
#endif
        // 3. Render the output
        renderProgram->Bind();

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // 4. Swap SSBO indices for next frame
        currentSSBOIndex = (currentSSBOIndex + 1) % 2;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
