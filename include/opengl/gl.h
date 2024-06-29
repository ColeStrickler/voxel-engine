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
#include "shader.h"
#include <string>
#include <iostream>
#include <functional> // for std::bind
#include "renderer.h"
#include "gui_manager.h"
#define DEFAULT_WINDOW_WIDTH 1200U
#define DEFAULT_WINDOW_HEIGHT 900U  
#define DEFUALT_MOVE_SPEED 10.0f
#define DEFAULT_MOUSE_SENSITIVITY 0.05f
#define DEFAULT_OPENGL_VERSION_MAJOR 4
#define DEFAULT_OPENGL_VERSION_MINOR 5
class GLManager;

class Camera
{
public:
    Camera(float screen_height, float screen_width, float speed, GLManager* manager);
   // Camera();
    ~Camera();
    void ChangeScreenDimensions(float width, float height);
    bool SetMVP(ShaderProgram* prog);
    std::string FetchLog();
    void DumpLog();
    void ChangeMoveSpeed(float speed);
    glm::vec3 GetPosition() const;
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
    void CameraHandleMouseMovement(float xoffset, float yoffset);
    void CameraHandleKey_W();
    void CameraHandleKey_A();
    void CameraHandleKey_S();
    void CameraHandleKey_D();
    void CameraHandleKey_SPACE();
    void UpdateCameraVectors();
private:
    // glm::mat4 m_Model; the model matrix will be set by the rendered objects
    glm::vec3 m_CameraPos;
    glm::vec3 m_CameraFront;
    glm::vec3 m_CameraUp;
    glm::vec3 m_CameraRight;
    glm::vec3 m_WorldUp;
    glm::mat4 m_View;
    glm::mat4 m_Projection;
    float m_Pitch;
    float m_Yaw;
    float m_MouseSensitivity;
    float m_MoveSpeed;
    float m_ScreenWidth;
    float m_ScreenHeight;
    std::string m_Log;
    GLManager* m_Manager;
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
    void RegisterKeyCallback(int key, std::function<void()> callback);
    Camera* GetCamera(){return &m_Camera;}
    static std::unordered_map<int, std::function<void()>> m_KeyCallbacks;
    void CalcDeltaTime() {float curr = glfwGetTime(); m_DeltaTime = curr-m_LastTime; m_LastTime=curr;};
    float GetDeltaTime() {return m_DeltaTime;}


    void PerFrame(); // put computation needing to be handled per frame here
private:
    float m_DeltaTime; // time between each frame rendering
    float m_LastTime;
    Camera m_Camera;
    static GLFWwindow* window;
    static int window_width;
    static int window_height;
    std::string m_glVersion;
    void GLFW_Init();
    void GLAD_Init();
    static void MouseScrollCallback(GLFWwindow* window, double xposIn, double yposIn);
    static void HandleAllKeyCallbacks(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseClickCallback(GLFWwindow* window, int button, int action, int mods);
    std::unordered_map<int, bool> m_KeyPressed;


    // mouse pos
    static bool m_FirstMouse;
    static float m_LastX;
    static float m_LastY;
};
#endif