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
#include "vertices.h"
#include "renderer.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
extern Logger logger;
extern GUI GUI_Manager;
extern Renderer renderer;
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
    logger.SetLogLevel(LOGLEVEL::LEVEL_INFO);
    logger.Log(LOGTYPE::INFO, "test");

    Shader vertex_shader(getcwd() + "/src/shaders/vertex.glsl", GL_VERTEX_SHADER, LightingModel::Phong);
    if (vertex_shader.CheckError() != ShaderError::NO_ERROR_OK)
        std::cout << vertex_shader.FetchLog();

    Shader fragment_shader(getcwd() + "/src/shaders/fragment.glsl", GL_FRAGMENT_SHADER, LightingModel::Phong);
    if (fragment_shader.CheckError() != ShaderError::NO_ERROR_OK)
        std::cout << fragment_shader.FetchLog();

    Shader lighting_fragment(getcwd() + "/src/shaders/ls_fragment.glsl", GL_FRAGMENT_SHADER, LightingModel::Phong);
    if (lighting_fragment.CheckError() != ShaderError::NO_ERROR_OK)
        std::cout << lighting_fragment.FetchLog();

    Shader lighting_vertex(getcwd() + "/src/shaders/ls_vertex.glsl", GL_VERTEX_SHADER, LightingModel::Phong);
    if (lighting_vertex.CheckError() != ShaderError::NO_ERROR_OK)
        std::cout << lighting_vertex.FetchLog();

    // check for shader compile errors
    ShaderProgram shaderProgram;
    ShaderProgram lightSource;
    shaderProgram.AddShader(&vertex_shader);
    shaderProgram.AddShader(&fragment_shader);

    lightSource.AddShader(&lighting_vertex);
    lightSource.AddShader(&lighting_fragment);
    // glAttachShader(shaderProgram, vertexShader);
    // glAttachShader(shaderProgram, fragmentShader);
    if (!shaderProgram.Compile())
    {
        return -1;
    }

    if (!lightSource.Compile())
    {
        return -1;
    }

    BufferLayout lighting_layout({BufferElement("COORDS", ShaderDataType::Float3, false),
                                  BufferElement("NORMALS", ShaderDataType::Float3, false)});
    VertexArray lva;
    VertexBuffer lighting_vbo(light_vertices, sizeof(light_vertices));
    lighting_vbo.SetLayout(lighting_layout);
    lva.AddVertexBuffer(lighting_vbo);

    // VertexBuffer
    BufferLayout layout({BufferElement("COORDS", ShaderDataType::Float3, false),
                         BufferElement("NORMALS", ShaderDataType::Float3, false)});
    VertexArray va;
    VertexBuffer vbo(light_vertices, sizeof(light_vertices));
    vbo.SetLayout(layout);
    // IndexBuffer ibo(indices, 6);
    va.AddVertexBuffer(vbo);
    va.SetCount(36);
    // va.AddIndexBuffer(ibo);
    Texture tex(getcwd() + "/src/textures/container.jpg", "container");

    RenderObject r_obj(&va, &vbo, &shaderProgram);

    renderer.AddRenderObject(&r_obj);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(gl.GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    float opacity = 0.5f;
    bool sign = false;

    // /shaderProgram.Bind();
    // /if(!shaderProgram.SetUniform1i("texture1", 0))
    // /{
    // /    printf("Could not bind texture1\n");
    // /}

    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)DEFAULT_WINDOW_WIDTH / (float)DEFAULT_WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::vec3 cube_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    model = glm::translate(model, cube_pos);
    view = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 ls_model = glm::mat4(1.0f);
    glm::vec3 ls_pos = glm::vec3(2.0f, 0.0f, 0.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 0.4f, 0.40f);
    ls_model = glm::translate(ls_model, ls_pos);

    while (!glfwWindowShouldClose(gl.GetWindow()))
    {
        // gl.CalcDeltaTime();
        gl.PerFrame();
        logger.WriteLogs();
        // model = glm::rotate(model, .1f, );
        r_obj.Rotate(glm::vec3(0.5f, 1.0f, 0.0f), .1f);
        // float time = glfwGetTime();

        // auto new_model = glm::translate(model, glm::vec3(cos(time), 0, 0));
        // input
        // -----
        // processInput(gl.GetWindow(), view);

        // render
        // ------

        // tell OpenGL a new frame is about to begin
        GUI_Manager.Begin();

        // draw our first triangle
        // glUseProgram(shaderProgram);

        // tex.SetTextureSlot(0);
        // tex.Bind();

        shaderProgram.Bind();
        shaderProgram.SetUniformVec3("lightColor", lightColor);
        shaderProgram.SetUniformVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        shaderProgram.SetUniformVec3("lightPos", ls_pos);
        shaderProgram.SetUniformVec3("viewPos", gl.GetCamera()->GetPosition());
        // shaderProgram.SetUniformMat4("model", model);
        if (!gl.UpdateCameraMVP(&shaderProgram))
        {
            std::cout << gl.GetCamera()->FetchLog() << std::endl;
            exit(-1);
        }
        // va.Bind();
        renderer.RenderAllObjects();

        // va.Bind();
        // glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        lightSource.Bind();
        lightSource.SetUniformMat4("model", ls_model);
        lightSource.SetUniformVec3("lightColor", lightColor);

        if (!gl.UpdateCameraMVP(&lightSource))
        {
            std::cout << gl.GetCamera()->FetchLog() << std::endl;
            exit(-1);
        }

        lva.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glBindVertexArray(0); // no need to unbind it every time

        ImVec2 tabBarSize(ImGui::GetIO().DisplaySize.x, 100);
        ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - tabBarSize.y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, tabBarSize.y), ImGuiCond_Always);
        ImGui::Begin("Log");
        GUI_Manager.RenderGUI();
        // Text that appears in the window
        // ImGui::Text("Hello there adventurer!");

        // ImGui::End();

        GUI_Manager.End();
        // Renders the ImGUI elements
        // ImGui::Render();
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}