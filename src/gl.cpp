#include "gl.h"

/*
    Declarations for GLManager static class variables
*/
GLFWwindow* GLManager::window = nullptr;
int GLManager::window_width = DEFAULT_WINDOW_WIDTH; 
int GLManager::window_height = DEFAULT_WINDOW_HEIGHT;




GLManager::GLManager()
{
    GLFW_Init();
    GLAD_Init();
    m_glVersion = std::string((char*)glGetString(GL_VERSION));
    //m_Camera = Camera(static_cast<float>(window_height), static_cast<float>(window_width));
}

GLManager::~GLManager()
{
    glfwTerminate();
}

void GLManager::SetWindowSize(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    GLManager::window_height = height;
    GLManager::window_width = width;
    //m_Camera.ChangeScreenDimensions(static_cast<float>(GLManager::window_height), static_cast<float>(GLManager::window_width));
}

GLFWwindow* GLManager::GetWindow()
{
    return GLManager::window;
}

void GLManager::SetDepthTesting(bool enable)
{
    // if depth testing is set, OpenGL will discard any pixels that are behind anoth in the fragment shader
    if (enable)
        glEnable(GL_DEPTH_TEST); 
    else
        glDisable(GL_DEPTH_TEST);
}

bool GLManager::UpdateCameraMVP(ShaderProgram *prog)
{
    return 0; //m_Camera.SetMVP(prog);
}

void GLManager::GLFW_Init()
{
    if (!glfwInit())
    {
        std::cout << "glfwInit() failed." << std::endl;
        glfwTerminate();
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, DEFAULT_OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, DEFAULT_OPENGL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLManager::window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (GLManager::window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, (GLFWframebuffersizefun)SetWindowSize);
}

void GLManager::GLAD_Init()
{
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
}

Camera::Camera(float screen_height, float screen_width) : m_Model(glm::mat4(1.0f)),  m_View(glm::mat4(1.0f))
{
    ChangeScreenDimensions(screen_width, screen_height);
}

Camera::~Camera()
{
    DumpLog();
}

void Camera::ChangeScreenDimensions(float width, float height)
{
    m_ScreenHeight = height;
    m_ScreenWidth = width;
    m_Projection = glm::perspective(glm::radians(45.0f), m_ScreenWidth/m_ScreenHeight, 0.1f, 100.0f);
}

bool Camera::SetMVP(ShaderProgram *prog)
{
    if (!prog->SetUniformMat4("model", m_Model))
    {
        m_Log += "Camera::SetMVP() --> failed to set model matrix.\n";
        return false;
    }
    
    if(!prog->SetUniformMat4("view", m_View))
    {
        m_Log += "Camera::SetMVP() --> failed to set view matrix.\n";
        return false;
    }
    if(!prog->SetUniformMat4("proj", m_Projection))
    {
        m_Log += "Camera::SetMVP() --> failed to set projection matrix.\n";
        return false;
    }
}

std::string Camera::FetchLog()
{
    return m_Log;
}

void Camera::DumpLog()
{
    m_Log.clear();
    m_Log.shrink_to_fit();
}
