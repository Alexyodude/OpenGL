#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Log.h"
#include "Renderer.h"

#include "Shader.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

//#include "tests/TestClearColor.h"

//#include "Player.h"

void Cursor_Pos_Callback(GLFWwindow* window, double xpos, double ypos);
void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int Window_Width = 1000;
int Window_Height = 500;
double xpos, ypos;

//Player player;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(Window_Width, Window_Height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
    }
    glfwSwapInterval(1);

    std::cout << glGetString(GL_VERSION) << std::endl;

    //glfwSetCursorPosCallback(window, Cursor_Pos_Callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //glfwSetKeyCallback(window, Key_Callback);
    {
        float vertex[] =
        {
           -50.0f,   0.0f, 0.0f, 0.0f, //0  
             0.0f, -50.0f, 1.0f, 0.0f, //1
            50.0f,   0.0f, 1.0f, 1.0f, //2
             0.0f,  50.0f, 0.0f, 1.0f, //3
        };

        unsigned int index[] =
        {
            0,1,2,
            2,3,0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray va;
        VertexBuffer vb(vertex, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(index, 6);

        glm::mat4 proj = glm::ortho(0.0f, (float)Window_Width, 0.0f, (float)Window_Height, -100.0f, 100.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);

        Texture texture("res/textures/test.jpg");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        ImGui::StyleColorsDark();

        glm::vec3 translationA(200, 200, 0);
        glm::vec3 translationB(200, 100, 0);

        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
                glm::mat4 mvp = proj * view * model;
                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);
                
                renderer.Draw(va, ib, shader);
            }

            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
                glm::mat4 mvp = proj * view * model;
                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);
                
                renderer.Draw(va, ib, shader);
            }

            //vertex[0] = xpos;
            //vertex[1] = ypos;
            //std::cout << vertex[0] << std::endl;

            //va.Bind();
            //ib.Bind();
            //vb.SetData(vertex, 4 * 4 * sizeof(float));

            {
                //static float f = 0.0f;
                //static int counter = 0;

                ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

                ImGui::SliderFloat3("TranslationA", &translationA.x, 0.0f, (float)Window_Width);
                ImGui::SliderFloat3("TranslationB", &translationB.x, 0.0f, (float)Window_Width);

                //ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
                //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
                //ImGui::Checkbox("Another Window", &show_another_window);

                //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

                //if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                //    counter++;
                //ImGui::SameLine();
                //ImGui::Text("counter = %d", counter);

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

static void Cursor_Pos_Callback(GLFWwindow* window, double xpos1, double ypos1)
{
    glfwGetFramebufferSize(window, &Window_Width, &Window_Height);
    xpos =  2 * (xpos1/(Window_Width) - 0.5f);
    ypos = -2 * (ypos1/(Window_Height) - 0.5f);
	std::cout << xpos << ", " << ypos << std::endl;
}

void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W) {
        std::cout << key << std::endl;
        //player.Input();
    }
}