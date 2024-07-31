#include "gl.h"

extern GLManager gl;
extern Renderer renderer;
extern GUI GUI_Manager;
/*
    Declarations for GLManager static class variables
*/
GLFWwindow *GLManager::window = nullptr;
int GLManager::window_width = DEFAULT_WINDOW_WIDTH;
int GLManager::window_height = DEFAULT_WINDOW_HEIGHT;
bool GLManager::m_FirstMouse = true;
float GLManager::m_LastX = 0.0f;
float GLManager::m_LastY = 0.0f;
std::unordered_map<int, std::function<void()>> GLManager::m_KeyCallbacks;

GLManager::GLManager() : m_DeltaTime(0.0f), m_LastTime(0.0f), m_Camera(Camera(static_cast<float>(window_height), static_cast<float>(window_width), DEFUALT_MOVE_SPEED, this)),
    m_ViewLock(false), m_bShowGlStats(false)
{
    GLFW_Init();
    GLAD_Init();
    m_glVersion = std::string((char *)glGetString(GL_VERSION));
    glfwSetKeyCallback(window, HandleAllKeyCallbacks);
    glfwSetMouseButtonCallback(window, MouseClickCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
    m_bCursorHidden = true;
    /*
        Register camera movements

        Pointers to member functions are handled differently, and cannot be handled as regular function pointers.

        We must bind them to a particular instance, so we bind them to m_Camera
    */
    RegisterKeyCallback(GLFW_KEY_W, std::bind(&Camera::CameraHandleKey_W, &m_Camera));
    RegisterKeyCallback(GLFW_KEY_A, std::bind(&Camera::CameraHandleKey_A, &m_Camera));
    RegisterKeyCallback(GLFW_KEY_S, std::bind(&Camera::CameraHandleKey_S, &m_Camera));
    RegisterKeyCallback(GLFW_KEY_D, std::bind(&Camera::CameraHandleKey_D, &m_Camera));
    RegisterKeyCallback(GLFW_KEY_ESCAPE, std::bind(&GLManager::HandleToggleCursorHidden, this));
    RegisterKeyCallback(GLFW_KEY_F3, std::bind(&GLManager::HandleDisplayProfilerStatistics, this));
    RegisterKeyCallback(GLFW_KEY_F2, std::bind(&GLManager::HandleShowGlStats, this));
    RegisterKeyCallback(GLFW_KEY_F1, std::bind(&GLManager::HandleToggleCursorVisible, this));
    glfwSetCursorPosCallback(window, MouseScrollCallback);
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
    gl.m_Camera.ChangeScreenDimensions(static_cast<float>(GLManager::window_height), static_cast<float>(GLManager::window_width));
}

GLFWwindow *GLManager::GetWindow()
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

void GLManager::SetStencilTesting(bool enable)
{
    if (enable) // should propbably dynamically schange glClear flags here
         glEnable(GL_STENCIL_TEST);
    else
         glDisable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); 
}

bool GLManager::UpdateCameraMVP(ShaderProgram *prog)
{
    return m_Camera.SetMVP(prog);
    
}

void GLManager::RegisterKeyCallback(int key, std::function<void()> callback)
{
    m_KeyCallbacks[key] = callback;
    m_KeyPressed[key] = false;
}

void GLManager::PerFrame()
{
    EMIT_PROFILE_TOKEN
    glfwPollEvents();
    for (auto &e : m_KeyPressed)
    {
        auto cb = m_KeyCallbacks[e.first];
        if (e.second && cb)
        {
            cb();
        }
    }

    CalcDeltaTime();
    UpdateStats();
    m_Camera.UpdateCameraVectors();
    m_Camera.GetViewMatrix();

    glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 
    


    DisplayGlStats();

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
    glfwSwapInterval(0);
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

void GLManager::MouseScrollCallback(GLFWwindow *window, double xposIn, double yposIn)
{
    if (gl.m_ViewLock)
        return;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (m_FirstMouse)
    {
        m_LastX = xpos;
        m_LastY = ypos;
        m_FirstMouse = false;
    }

    float xoffset = xpos - m_LastX;
    float yoffset = m_LastY - ypos; // reversed since y-coordinates go from bottom to top

    m_LastX = xpos;
    m_LastY = ypos;

    gl.m_Camera.CameraHandleMouseMovement(xoffset, yoffset);
}

void GLManager::HandleAllKeyCallbacks(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    gl.m_KeyPressed[key] = action;
}

void GLManager::MouseClickCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (action != GLFW_PRESS) // Check if the mouse button was pressed
        return;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos); // Get cursor position

    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height); // Get framebuffer size

    GLubyte color[4];
    GLfloat depth;
    GLuint index;

    // Read pixel data from the framebuffer
    glReadPixels((int)xpos, window_height - (int)ypos - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
    glReadPixels((int)xpos, window_height - (int)ypos - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    glReadPixels((int)xpos, window_height - (int)ypos - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

    glm::vec4 viewport = glm::vec4(0, 0, window_width, window_height);
    glm::vec3 wincoord = glm::vec3(xpos, window_height - ypos - 1, depth);
    glm::vec3 objcoord = glm::unProject(wincoord, gl.m_Camera.GetViewMatrix(), gl.m_Camera.GetProjectionMatrix(), viewport);

    RenderObject *selectedObject = renderer.FindClosestObject(objcoord);
    if (selectedObject)
        GUI_Manager.HandleObjectSelection(selectedObject);
}

void GLManager::HandleToggleCursorHidden()
{
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 
   m_ViewLock = true;
}

void GLManager::HandleToggleCursorVisible()
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    m_ViewLock = false;
}

void GLManager::HandleShowGlStats()
{
 // dummy
}

void GLManager::DisplayGlStats()
{
    auto KEYSTATUS = m_KeyPressed[GLFW_KEY_F2];

    auto screen_height = gl.GetCamera()->GetScreenHeight();
    auto text_color = glm::vec3(0.05f, 1.0f, 0.3f);
    if (KEYSTATUS == GLFW_PRESS || KEYSTATUS == GLFW_REPEAT) 
    {
        std::string FPS = "FPS: " + std::to_string(m_GlStats.m_FramesPerSecond);
        renderer.RenderText(FPS.c_str(), 30.0f, screen_height - 30.0f, 0.6f, text_color);

        std::string MEMORY_USAGE = "Memory Usage: " + std::to_string(util::GetMemoryUsageKb()) +"Kb";
        renderer.RenderText(MEMORY_USAGE.c_str(), 30.0f, screen_height - 60.0f, 0.6f, text_color);
    }
    
}


void GLManager::HandleDisplayProfilerStatistics()
{
    auto KEYSTATUS = m_KeyPressed[GLFW_KEY_F3];
    if (KEYSTATUS == GLFW_PRESS || KEYSTATUS == GLFW_REPEAT) 
    {
         GUI_Manager.m_bShowProfilerStatistics = true;
    }
}

void GLManager::UpdateStats()
{
    m_GlStats.m_PrevTimes.push_back(m_DeltaTime);
    // lets encapsulate this into a method of GlStats
    while (m_GlStats.m_PrevTimes.size() > 5)
        m_GlStats.m_PrevTimes.pop_front();
    float times = 0.0f;
    for (auto& time : m_GlStats.m_PrevTimes)
        times += time;
    times /= 5;
    m_GlStats.m_FramesPerSecond = 1.0f / times;
}



Camera::Camera(float screen_height, float screen_width, float speed, GLManager *manager) : m_Manager(manager)
{
    ChangeScreenDimensions(screen_width, screen_height);
    ChangeMoveSpeed(speed);
    m_CameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
    m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f) + m_CameraPos;
    m_CameraUp = glm::cross(m_CameraFront, m_CameraRight);
    m_CameraRight = glm::normalize(glm::cross(m_CameraUp, m_CameraFront));
    m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    m_MouseSensitivity = DEFAULT_MOUSE_SENSITIVITY;
    m_Pitch = 0.0f;
    m_Yaw = -90.0f;
    
    UpdateCameraVectors();
    GetViewMatrix();
}

Camera::~Camera()
{
    DumpLog();
}

void Camera::ChangeScreenDimensions(float width, float height)
{
    m_ScreenHeight = height;
    m_ScreenWidth = width;
    m_Projection = glm::perspective(glm::radians(45.0f), m_ScreenWidth / m_ScreenHeight, 0.1f, 200.0f);
    m_Ortho = glm::ortho(0.0f, width, 0.0f, height);
}

bool Camera::SetMVP(ShaderProgram *prog)
{
    /*
        Model matrix will be set by the objects themselves
    */
    // if (!prog->SetUniformMat4("model", m_Model))
    //{
    //     m_Log += "Camera::SetMVP() --> failed to set model matrix.\n";
    //     return false;
    // }

    if (!prog->SetUniformMat4("view", m_View))
    {
        m_Log += "Camera::SetMVP() --> failed to set view matrix.\n";
        std::cout << m_Log;
        return false;
    }
    if (!prog->SetUniformMat4("projection", m_Projection))
    {
        m_Log += "Camera::SetMVP() --> failed to set projection matrix.\n";
        return false;
    }
    return true;
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

void Camera::ChangeMoveSpeed(float speed)
{
    m_MoveSpeed = speed;
    // m_View = glm::lookAt(m_CameraPos, m_CameraPos+m_CameraFront, m_CameraUp);
}

glm::vec3 Camera::GetPosition() const
{
    return m_CameraPos;
}

glm::mat4 Camera::GetViewMatrix()
{
    m_View = glm::lookAt(m_CameraPos, m_CameraPos + m_CameraFront, m_CameraUp);
    return m_View;
}

glm::mat4 Camera::GetProjectionMatrix()
{
    return m_Projection;
}

void Camera::CameraHandleMouseMovement(float xoffset, float yoffset)
{
    xoffset *= m_MouseSensitivity;
    yoffset *= m_MouseSensitivity;

    m_Yaw += xoffset;
    m_Pitch += yoffset;

    if (m_Pitch > 89.0f)
        m_Pitch = 89.0f;
    if (m_Pitch < -89.0f)
        m_Pitch = -89.0f;
    UpdateCameraVectors();
    //GetViewMatrix();
}

void Camera::CameraHandleKey_W()
{
    float delta_time = m_Manager->GetDeltaTime();
    float speed = delta_time * m_MoveSpeed;
    m_CameraPos += (speed * m_CameraFront);
    // UpdateCameraVectors();
    // GetViewMatrix();
    // printf("%f, %f, %f\n", m_CameraPos[0],  m_CameraPos[1],  m_CameraPos[2]);
}

void Camera::CameraHandleKey_A()
{
    float delta_time = m_Manager->GetDeltaTime();
    float speed = delta_time * m_MoveSpeed;
    m_CameraPos -= (m_CameraRight * speed);
    // UpdateCameraVectors();
    // GetViewMatrix();
}

void Camera::CameraHandleKey_S()
{
    float delta_time = m_Manager->GetDeltaTime();
    float speed = delta_time * m_MoveSpeed;
    m_CameraPos -= (speed * m_CameraFront);
    // UpdateCameraVectors();
    // GetViewMatrix();
}

void Camera::CameraHandleKey_D()
{
    float delta_time = m_Manager->GetDeltaTime();
    float speed = delta_time * m_MoveSpeed;
    /*
        Get a vector that is orthogonal to both the CameraUp and the direction the camera is pointing
        via the cross product

        We add to move to the right
    */
    m_CameraPos += (speed * m_CameraRight);
    // UpdateCameraVectors();
    // GetViewMatrix();
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_CameraFront = glm::normalize(front);
    // also re-calculate the Right and Up vector
    m_CameraRight = glm::normalize(glm::cross(m_CameraFront, m_WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_CameraUp = glm::normalize(glm::cross(m_CameraRight, m_CameraFront));
}
