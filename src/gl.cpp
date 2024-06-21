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
}

GLFWwindow* GLManager::GetWindow()
{
    return GLManager::window;
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
