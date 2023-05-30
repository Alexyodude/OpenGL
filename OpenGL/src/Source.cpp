//#define ImGui_DEBUG_TestTexture
#define ImGui_DEBUG
#define KEY_DEBUG

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

#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"

#include "Layer.h"

#include "EventListener.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(Window_Width, Window_Height, "Hello Wurld", NULL, NULL);
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

    glfwSetCursorPosCallback(window, Cursor_Pos_Callback);
    glfwSetKeyCallback(window, Key_Callback);
    glfwSetWindowSizeCallback(window, Window_Size_Callback);

    {
        GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        
        //Renderer renderer;

        #ifdef ImGui_DEBUG
            ImGui::CreateContext();
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 330");
            ImGui::StyleColorsDark();
        #endif
        #ifdef ImGui_DEBUG_TestTexture
            test::Test* currentTest = nullptr;
            test::TestMenu* testMenu = new test::TestMenu(currentTest);
            currentTest = testMenu;

            testMenu->RegisterTest<test::TestClearColor>("Clear Color");
            testMenu->RegisterTest<test::TestTexture2D>("Texture 2D");
        #endif
        std::unique_ptr <Texture> T1 = std::make_unique<Texture>("res/textures/test.jpg");
        std::unique_ptr <Texture> T2 = std::make_unique<Texture>("res/textures/earth.jpg");
        std::unique_ptr <Layer> layer = std::make_unique<Layer>();
        T1->Bind(1);
        T2->Bind(2);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {  
            #ifdef ImGui_DEBUG
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
            #endif
           // std::unique_ptr<SandBox> sandBox = std::make_unique<SandBox>(&renderer);
            //sandBox->SetRenderer(&renderer);
            //sandBox->AddQuad(10+sandBox->index1.size(), 10, 2);
            layer->OnRender();
            layer->OnEvent(KEY);

            #ifdef ImGui_DEBUG_TestTexture
                if (currentTest) 
                {
                    currentTest->OnUpdate(0.0f); //Updates the test
                    currentTest->OnRender(); //Renders the test

                    ImGui::Begin("Test"); //Begins the test ui window
                    if (currentTest != testMenu) //If the current test is not the test menu
                        if (ImGui::Button("<-")) //If the button is pressed
                        {
                            delete currentTest; //Deletes the current test
                            currentTest = testMenu; //Sets the current test to the test menu
                        }

                    currentTest->OnImGuiRender(); //Renders the test ui slider, button, etc.

                    ImGui::End(); //Ends the test ui window
                }
            #endif
            #ifdef ImGui_DEBUG
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            #endif
            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
        #ifdef ImGui_DEBUG_TestTexture
        delete currentTest; //Deletes the current test
        if (currentTest != testMenu) //If the current test is not the test menu
			delete testMenu; //Deletes the test menu
        #endif
    }
    #ifdef ImGui_DEBUG
        ImGui_ImplOpenGL3_Shutdown(); 
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    #endif

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
