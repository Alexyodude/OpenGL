#pragma once

#include <iostream>

#include "Renderer.h"
#include "GLFW/glfw3.h"

bool KEY[128];

void Cursor_Pos_Callback(GLFWwindow* window, double xpos, double ypos)
{
    glfwGetFramebufferSize(window, &Window_Width, &Window_Height);
    //xpos =  2 * (xpos1/(Window_Width) - 0.5f);
    //ypos = -2 * (ypos1/(Window_Height) - 0.5f);
    std::cout << xpos << ", " << ypos << std::endl;
}

void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS) {
            #ifdef KEY_DEBUG 
            std::cout << "Pressed: " << key << std::endl;
            #endif
            KEY[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            #ifdef KEY_DEBUG 
            std::cout << "Released: " << key << std::endl;
            #endif
			KEY[key] = false;
		}
    }else if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) {
            #ifdef KEY_DEBUG 
            std::cout << "Pressed: " << key << std::endl;
            #endif
            KEY[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            #ifdef KEY_DEBUG 
            std::cout << "Released: " << key << std::endl;
            #endif
            KEY[key] = false;
        }
    }    
    
    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) {
            #ifdef KEY_DEBUG 
			std::cout << "Pressed: " << key << std::endl;
            #endif
            KEY[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            #ifdef KEY_DEBUG
			std::cout << "Released: " << key << std::endl;
			#endif
            KEY[key] = false;
        }
    }else if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS) {
            #ifdef KEY_DEBUG 
			std::cout << "Pressed: " << key << std::endl;
            #endif
			KEY[key] = true;
		}
        else if (action == GLFW_RELEASE) {
            #ifdef KEY_DEBUG 			
            std::cout << "Released: " << key << std::endl;
			#endif
			KEY[key] = false;
		}
	}

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void Window_Size_Callback(GLFWwindow* window, int width, int height)
{
    std::cout << width << ", " << height << std::endl;
    Window_Width = width;
    Window_Height = height;
    glViewport(0, 0, width, height);
}