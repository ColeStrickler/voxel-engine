#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include "gl.h"
#include "shader.h"
#include "glbuffer.h"
#include <filesystem>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

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

    GLManager gl;
    
   // std::cout << "OpenGL version: " << version << std::endl;


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
    printf("hjere!\n");
    //glAttachShader(shaderProgram, vertexShader);
    //glAttachShader(shaderProgram, fragmentShader);
    if (!shaderProgram.Compile())
    {
        std::cout << shaderProgram.FetchLog();
        return -1;
    }
    printf("x\n");
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
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    }; 


    //VertexBuffer 


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    VertexBuffer vbo(vertices, sizeof(vertices));
    printf("x\n");
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    vbo.Bind();
    printf("x\n");
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    vbo.Unbind();
    printf("x\n");

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------

    printf("here!\n");
    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(gl.GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
    float opacity = 0.5f;
    bool sign = false;
    while (!glfwWindowShouldClose(gl.GetWindow()))
    {
        if (opacity >= 1.0f || opacity <= 0.0f)
            sign = !sign;

        if (sign)
            opacity += 0.01f;
        else
            opacity -= 0.01f; 
        // input
        // -----
        processInput(gl.GetWindow());

        // render
        // ------
        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // tell OpenGL a new frame is about to begin
        ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


        // draw our first triangle
        //glUseProgram(shaderProgram);
        shaderProgram.Bind();
        if (!shaderProgram.SetUniform1f("opacity", opacity))
            exit(-1);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time 



        ImGui::Begin("My name is window, ImGUI window");
		// Text that appears in the window
		ImGui::Text("Hello there adventurer!");
		// Checkbox that appears in the window
		//ImGui::Checkbox("Draw Triangle", &drawTriangle);
		// Slider that appears in the window
		//ImGui::SliderFloat("Size", &size, 0.5f, 2.0f);
		// Fancy color editor that appears in the window
		//ImGui::ColorEdit4("Color", color);
		// Ends the windowfloat)*9
		ImGui::End();


        // Renders the ImGUI elements
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(gl.GetWindow());
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
   // glDeleteBuffers(1, &VBO);
    

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}