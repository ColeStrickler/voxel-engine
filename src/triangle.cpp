#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "gl.h"
#include "shader.h"
#include "glbuffer.h"
#include "glvertexarray.h"
#include <filesystem>
#include "texture.h"
#include "logger.h"
#include "gui_manager.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
extern Logger logger;
extern GUI GUI_Manager;
GLManager gl;
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
std::string getcwd()
{
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::cout << "Current working directory: " << currentPath << std::endl;

    // If you need it explicitly as a std::string
    return currentPath.string();
}


int main()
{

    
    gl.SetDepthTesting(true);

    GUI_Manager.RegisterLogTarget(&logger);

   // std::cout << "OpenGL version: " << version << std::endl;
    logger.SetLogLevel(LOGLEVEL::LEVEL_INFO);
    logger.Log(LOGTYPE::INFO, "test");

    // glfw: initialize and configure
    // ------------------------------
    /*
    
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

*/
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
   // unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
   // glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
   // glCompileShader(vertexShader);

    Shader vertex_shader(getcwd() + "/src/shaders/vertex.glsl", GL_VERTEX_SHADER);
    if (vertex_shader.CheckError() != ShaderError::NO_ERROR_OK)
        std::cout << vertex_shader.FetchLog();

    Shader fragment_shader(getcwd() + "/src/shaders/fragment.glsl", GL_FRAGMENT_SHADER);
    if (fragment_shader.CheckError() != ShaderError::NO_ERROR_OK)
        std::cout << fragment_shader.FetchLog();
    // check for shader compile errors
    int success;
    char infoLog[512];
    //glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    //if (!success)
    //{
    //    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    //    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
   // }
    // fragment shader
   // unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
   // glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
   // glCompileShader(fragmentShader);
   // // check for shader compile errors
   // glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
   // if (!success)
   // {
   //     glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
   //     std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
   // }
    // link shaders
    ShaderProgram shaderProgram;
    shaderProgram.AddShader(&vertex_shader);
    shaderProgram.AddShader(&fragment_shader);
    //glAttachShader(shaderProgram, vertexShader);
    //glAttachShader(shaderProgram, fragmentShader);
    if (!shaderProgram.Compile())
    {
        return -1;
    }

    //vertex_shader.Attach(shaderProgram);
    //fragment_shader.Attach(shaderProgram);

    //glLinkProgram(shaderProgram);
    //// check for linking errors
    //glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    //if (!success) {
    //    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    //    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    //    exit(-1);
    //}
    //glDeleteShader(vertexShader);
    //glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    //VertexBuffer 
    BufferLayout layout({BufferElement("COORDS", ShaderDataType::Float3, false),\
    BufferElement("COORDS", ShaderDataType::Float2, false) });
    VertexArray va;
    VertexBuffer vbo(vertices, sizeof(vertices));
    vbo.SetLayout(layout);
   // IndexBuffer ibo(indices, 6);
    va.AddVertexBuffer(vbo);
   // va.AddIndexBuffer(ibo);
    Texture tex(getcwd()+"/src/textures/container.jpg", "container");

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
   // glBindVertexArray(VAO);
    //vbo.Bind();
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
   // vbo.Unbind();

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    //glBindVertexArray(0); 

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------

    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(gl.GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
    float opacity = 0.5f;
    bool sign = false;

    shaderProgram.Bind();
    if(!shaderProgram.SetUniform1i("texture1", 0))
    {
        printf("Could not bind texture1\n");
    }

    glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::perspective(glm::radians(45.0f), (float)DEFAULT_WINDOW_WIDTH/(float)DEFAULT_WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::vec3 cube_pos = glm::vec3(0.0f, 0.0f, 0.0f);


    model = glm::translate(model, cube_pos);
    view  = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
    while (!glfwWindowShouldClose(gl.GetWindow()))
    {
        //gl.CalcDeltaTime();
        gl.PerFrame();
        logger.WriteLogs();
        model = glm::rotate(model, .1f, glm::vec3(0.5f, 1.0f, 0.0f));
        // input
        // -----
       // processInput(gl.GetWindow(), view);

        // render
        // ------
        
        // tell OpenGL a new frame is about to begin
        GUI_Manager.Begin();


        // draw our first triangle
        //glUseProgram(shaderProgram);
        
        
        
        tex.SetTextureSlot(0);
        tex.Bind();
        
        
        
        shaderProgram.Bind();
        shaderProgram.SetUniformMat4("model", model);
        if(!gl.UpdateCameraMVP(&shaderProgram))
        {
            std::cout << gl.GetCamera()->FetchLog() << std::endl;
            exit(-1);
        }
        shaderProgram.SetUniformBool("showTexture", sign);

        va.Bind();
        //glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
       // glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // glBindVertexArray(0); // no need to unbind it every time 

        ImVec2 tabBarSize(ImGui::GetIO().DisplaySize.x, 100);  
        ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - tabBarSize.y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, tabBarSize.y), ImGuiCond_Always);
        ImGui::Begin("Log");
        GUI_Manager.RenderGUI();
		// Text that appears in the window
		//ImGui::Text("Hello there adventurer!");
        
		//ImGui::End();

        GUI_Manager.End();
        // Renders the ImGUI elements
		//ImGui::Render();
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(gl.GetWindow());
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
   // glDeleteVertexArrays(1, &VAO);
   // glDeleteBuffers(1, &VBO);
    

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}