#ifndef GLMANAGER_H
#define GLMANAGER_H
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader.h>
#include <string>
#include <iostream>

#define DEFAULT_WINDOW_WIDTH 800U
#define DEFAULT_WINDOW_HEIGHT 600U  
#define DEFAULT_OPENGL_VERSION_MAJOR 4
#define DEFAULT_OPENGL_VERSION_MINOR 5


class Camera
{
public:
    Camera(float screen_height, float screen_width);
    ~Camera();
    void ChangeScreenDimensions(float width, float height);
    bool SetMVP(ShaderProgram* prog);
    std::string FetchLog();
    void DumpLog();
private:
    glm::mat4 m_Model; // we will need to modify these, not how we do it
    glm::mat4 m_View;
    glm::mat4 m_Projection;
    float m_ScreenWidth;
    float m_ScreenHeight;
    std::string m_Log;
};



// This class is used to manage the OpenGL state machine
class GLManager
{
public:
    GLManager();
    ~GLManager();
    static void SetWindowSize(GLFWwindow* window, int width, int height);
    GLFWwindow* GetWindow();
    void SetDepthTesting(bool enable);
    bool UpdateCameraMVP(ShaderProgram* prog);
private:
  //  static Camera m_Camera;
    static GLFWwindow* window;
    static int window_width;
    static int window_height;
    std::string m_glVersion;
    void GLFW_Init();
    void GLAD_Init();
};
#endif