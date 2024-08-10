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

GLuint createTexture(int width, int height, const void* data) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return texture;
}

void checkFramebufferStatus() {
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete: " << status << std::endl;
    }
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(width, height, "Game of Life", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewInit();

    // Create shader program
    std::unique_ptr<Shader> program = std::make_unique<Shader>("res/shaders/vertex.shader", "res/shaders/fragment.shader");
    program->Bind();

    // Initialize grid with random state
    std::vector<float> initialState(gridWidth * gridHeight, 0.0f);
    for (int i = 0; i < gridWidth * gridHeight; ++i) {
        initialState[i] = (rand() % 5 == 0) ? 1.0f : 0.0f;
    }

    // Create textures
    GLuint textures[2];
    textures[0] = createTexture(gridWidth, gridHeight, initialState.data());
    textures[1] = createTexture(gridWidth, gridHeight, nullptr);

    // Create FBO
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[0], 0);
    checkFramebufferStatus();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create VAO and VBO for quad
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    float quadVertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    // Set uniforms
    program->Bind();
    program->SetUniform1i("u_Texture", 0);
    program->SetUniform2f("u_Resolution", gridWidth, gridHeight);

    int currentTextureIndex = 0;

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
            std::string title = "Game of Life - FPS: " + std::to_string(fps) + " - " + std::to_string(milliseconds / frameCount) + " ms/frame";
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            milliseconds = 0.0f;
            lastTime = currentTime;
        }

        // Render to FBO
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[(currentTextureIndex + 1) % 2], 0);
        glViewport(0, 0, gridWidth, gridHeight);
        //glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[currentTextureIndex]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Swap textures
        currentTextureIndex = (currentTextureIndex + 1) % 2;

        // Render to screen
        glViewport(0, 0, width, height);
        //glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, textures[currentTextureIndex]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
