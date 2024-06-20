#ifndef GLMANAGER_H
#define GLMANAGER_H
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <iostream>

#define DEFAULT_WINDOW_WIDTH 800U
#define DEFAULT_WINDOW_HEIGHT 600U  
#define DEFAULT_OPENGL_VERSION_MAJOR 3
#define DEFAULT_OPENGL_VERSION_MINOR 3



// This class is used to manage the OpenGL state machine
class GLManager
{
public:
    GLManager();
    ~GLManager();
    static void SetWindowSize(GLFWwindow* window, int width, int height);
    GLFWwindow* GetWindow();

private:
    static GLFWwindow* window;
    static int window_width;
    static int window_height;
    void GLFW_Init();
    void GLAD_Init();
};
#endif