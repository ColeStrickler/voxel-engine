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
#include "util.h"
#include "model_loader.h"
#include "block.h"
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
extern Logger logger;
extern GUI GUI_Manager;
extern Renderer renderer;
GLManager gl;
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;






int main()
{
    gl.SetDepthTesting(true);
    gl.SetStencilTesting(true);
    GUI_Manager.RegisterLogTarget(&logger);
    logger.SetLogLevel(LOGLEVEL::LEVEL_INFO);
    logger.Log(LOGTYPE::INFO, "test");

    Shader vertex_shader(util::getcwd() + "/src/shaders/vertex.glsl", GL_VERTEX_SHADER);
    if (vertex_shader.CheckError() != ShaderError::NO_ERROR_OK)
        logger.Log(LOGTYPE::ERROR, vertex_shader.FetchLog());

    Shader fragment_shader(util::getcwd() + "/src/shaders/fragment.glsl", GL_FRAGMENT_SHADER);
    if (fragment_shader.CheckError() != ShaderError::NO_ERROR_OK)
        logger.Log(LOGTYPE::ERROR, fragment_shader.FetchLog());


    // check for shader compile errors
    ShaderProgram shaderProgram;
    shaderProgram.AddShader(&vertex_shader);
    shaderProgram.AddShader(&fragment_shader);
;
    // glAttachShader(shaderProgram, vertexShader);
    // glAttachShader(shaderProgram, fragmentShader);
    renderer.SetLightingModel(LightingModel::Phong);


    if (!shaderProgram.Compile())
    {
        return -1;
    }
  


    BufferLayout* lighting_layout = new BufferLayout({new BufferElement("COORDS", ShaderDataType::Float3, false),
         new BufferElement("NORMALS", ShaderDataType::Float3, false),
          new BufferElement("TEXCOORDS", ShaderDataType::Float2, false) });
    VertexArray* lva = new VertexArray;
    VertexBuffer* lighting_vbo = new VertexBuffer(tvertices, sizeof(tvertices));
    lighting_vbo->SetLayout(lighting_layout);
    lva->AddVertexBuffer(lighting_vbo);

    RenderObject* l_obj = new RenderObject(lva, lighting_vbo, &shaderProgram, OBJECTYPE::PointLightSource);
    l_obj->SetPosition({0.0f, 0.0f, 0.0f});
    l_obj->m_Light.direction = glm::vec3({0.0, 0.0, 0.0});



    auto dirt_vertices = Block::GenBlockVertices(BlockType::Dirt);
    for (auto& v: dirt_vertices)
        PrintBlockVertex(v);


    BufferLayout* tex_layout = new BufferLayout({new BufferElement("COORDS", ShaderDataType::Float3, false),
          new BufferElement("NORMALS", ShaderDataType::Float3, false),new BufferElement("TEXCOORDS", ShaderDataType::Float2, false) });
    VertexArray* tva = new VertexArray;
    IndexBuffer* iva = new IndexBuffer((uint32_t*)cubeIndices.data(), BLOCK_INDICES_COUNT);
    VertexBuffer* tex_vbo = new VertexBuffer((float*)dirt_vertices.data(), BLOCK_VERTICES_SIZE);
    tex_vbo->SetLayout(tex_layout);
    tva->AddVertexBuffer(tex_vbo);
    tva->AddIndexBuffer(iva);
    RenderObject* t_obj = new RenderObject(tva, tex_vbo, &shaderProgram, iva, OBJECTYPE::TexturedObject);
    l_obj->SetPosition({0.0, 0.0, 0.0});
    

    Texture* spec = new Texture("/home/cole/Documents/voxel-engine/src/textures/texture_atlas.png", "spec");
    Texture* diff = new Texture("/home/cole/Documents/voxel-engine/src/textures/texture_atlas.png", "diff");
    t_obj->m_TexturedObject.AddDiffuseMap(diff);
    t_obj->m_TexturedObject.AddSpecularMap(spec);
    t_obj->m_TexturedObject.Shininess = 64.0f;



    //renderer.AddRenderObject(t_obj);
    //renderer.AddRenderObject(l_obj);

    for (int i = -20; i < 20; i += 4)
    {
        for (int j = -20; j < 20; j += 4)
        {
            auto obj = l_obj->Duplicate();
            obj->Translate({1.0*i, 5.0f, 1.0*j});
            renderer.AddRenderObject(obj);
        }

    }


    for (int i = -0; i < 1; i += 1)
    {
        for (int j = -0; j < 1; j += 1)
        {
            for (int x = 0; x < 1; x++)
            {
                
            }
        }

    }
                auto xobj = t_obj->Duplicate();
                xobj->Translate({1.0f, 0.0f, 1.0f});
                renderer.AddRenderObject(xobj);
    

    
    



    // va.AddIndexBuffer(ibo);
   //Texture tex(getcwd() + "/src/textures/container.jpg", "container");





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

    //ModelImporter* import = new ModelImporter(&shaderProgram);
    //import->LoadModel("/home/cole/Documents/voxel-engine/include/dev/models/mech_tank/scene.gltf");
    //auto mesh_model = import->ExportCurrentModel();
    //auto obj = new RenderObject(&shaderProgram, mesh_model);
    //for (int i = 0; i < 25; i++)
    //{
    //    auto xobj = obj->Duplicate();
    //    xobj->SetPosition(glm::vec3(50 *util::Random(), 50 *util::Random(), 50 *util::Random()));
    //    xobj->Rotate(glm::vec3(util::Random(), util::Random(), util::Random()), 180.0f * util::Random());
    //    renderer.AddRenderObject(xobj);
    //}

    

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
       // r_obj.Rotate(glm::vec3(0.5f, 1.0f, 0.0f), .1f);
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

       // shaderProgram.Bind();
       // shaderProgram.SetUniformVec3("lightColor", lightColor);
       // shaderProgram.SetUniformVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
       // shaderProgram.SetUniformVec3("lightPos", ls_pos);
       // shaderProgram.SetUniformVec3("viewPos", gl.GetCamera()->GetPosition());
        // shaderProgram.SetUniformMat4("model", model);
        
       


        // va.Bind();
      
       
        
        renderer.RenderAllObjects();
       

        // va.Bind();
        // glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

      //  lightSource.Bind();
      //  lightSource.SetUniformMat4("model", ls_model);
      //  lightSource.SetUniformVec3("lightColor", lightColor);

       // if (!gl.UpdateCameraMVP(&lightSource))
       // {
       //     std::cout << gl.GetCamera()->FetchLog() << std::endl;
       //     exit(-1);
       // }
//
       // lva.Bind();
       // glDrawArrays(GL_TRIANGLES, 0, 36);

        // glBindVertexArray(0); // no need to unbind it every time

        
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
        glfwPollEvents();
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