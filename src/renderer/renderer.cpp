#include "renderer.h"
Renderer renderer;
extern Logger logger;
extern GLManager gl;
Renderer::Renderer()
{
    m_RenderObjects.reserve(100);
}

Renderer::~Renderer()
{
}


void Renderer::AddRenderObject(RenderObject *obj)
{
    // acquire RenderLock whenever modifying the render objects list

    std::unique_lock lock(m_RenderLock);
    switch(obj->GetType())
    {
        case OBJECTYPE::RegularMaterial: m_RenderObjects.push_back(obj); break;
        case OBJECTYPE::PointLightSource: m_LightingObjects.push_back(obj); break;
        case OBJECTYPE::TexturedObject: m_RenderObjects.push_back(obj); break;
        case OBJECTYPE::ComplexModelObject: m_RenderObjects.push_back(obj); break;
        case OBJECTYPE::ChunkMesh: m_RenderObjects.push_back(obj); break;
        default:
            logger.Log(LOGTYPE::WARNING, "Renderer::AddRenderObject() --> attempted to add RenderObject with invalid OBJECTTYPE.\n");
    }
    

}

void Renderer::RenderAllObjects()
{
    EMIT_PROFILE_TOKEN

    std::unique_lock lock(m_RenderLock);

    HandleLightSources();
    
    auto it = m_RenderObjects.begin();
    while (it  != m_RenderObjects.end())
    {
        auto obj = *it;
        if (obj->Render())
        {
            it = m_RenderObjects.erase(it); // marked for deletion
            delete obj;

        }
        else
            it++;
    }
    //int i = m_RenderObjects.size();
    //for (int x = 0; x < i;  x++)
    //{
    //    auto& obj = m_RenderObjects[x];
    //    obj->Render();
    //}
    lock.unlock();
}

/*
    Traverses the RenderObject list and uses a simple distance formula to get the closest one

    If there are no objects returns nullptr
*/
RenderObject *Renderer::FindClosestObject(const glm::vec3 &pos)
{
    RenderObject* ret_obj = nullptr;
    float closest = 10e10;
    std::unique_lock lock(m_RenderLock);
    for (auto& obj: m_RenderObjects)
    {
        float dist = glm::distance(pos, obj->GetPosition());
        if (dist < closest && dist < MOUSE_CLICK_OBJ_SEL_THRESHOLD)
        {
            ret_obj = obj;
            closest = dist;
        }
    }

    for (auto& obj : m_LightingObjects)
    {
        float dist = glm::distance(pos, obj->GetPosition());
        if (dist < closest && dist < MOUSE_CLICK_OBJ_SEL_THRESHOLD)
        {
            ret_obj = obj;
            closest = dist;
        }
    }
    

    return ret_obj;
}



/*
    This function will render the light sources with the appropriate shaders and then
    set the appropriate uniforms for calculations by the other objects
*/
void Renderer::HandleLightSources()
{

    if (!m_LightingObjects.size())
        printf("not found!\n");

    int point_lights = 0;
    for (auto& obj: m_LightingObjects)
    {
        obj->Render();
        CalculateAndSetLightingUniforms(obj, point_lights);
    }
    auto prog = m_LightingObjects[0]->GetShaderProgram();
    prog->SetUniform1i("UsedPointLights", point_lights);
}

void Renderer::CalculateAndSetLightingUniforms(RenderObject* obj, int& point_lights)
{
    switch(obj->GetType())
    {
        case OBJECTYPE::DirectionalLightSource:
        {
            Light* light = &obj->m_Light;
            std::string format = "dirLight";
            std::string direction = format + ".direction";
            std::string position = format + ".position";
            std::string ambient = format + ".ambient";
            std::string diffuse = format + ".diffuse";
            std::string specular = format + ".specular";
            ShaderProgram* prog = obj->GetShaderProgram();

            prog->SetUniformVec3(ambient, light->ambient);
            prog->SetUniformVec3(diffuse, light->diffuse);
            prog->SetUniformVec3(specular, light->specular);
            prog->SetUniformVec3(direction, light->direction);
            prog->SetUniformVec3(position, obj->GetPosition());
            // shall we add attenuation for these?
        }
        case OBJECTYPE::PointLightSource:
        {
            if (point_lights >= MAX_POINT_LIGHTS)
                return;
            Light* light = &obj->m_Light;
            std::string format = "pointLights[" + std::to_string(point_lights) + "]";
            std::string constant  = format + ".constant";
            std::string linear = format + ".linear";
            std::string quadratic = format + ".quadratic";
            std::string ambient = format + ".ambient";
            std::string diffuse = format + ".diffuse";
            std::string specular = format + ".specular";
            std::string position = format + ".position";
            ShaderProgram* prog = obj->GetShaderProgram();

            prog->SetUniformVec3(ambient, light->ambient);
            prog->SetUniformVec3(diffuse, light->diffuse);
            prog->SetUniformVec3(specular, light->specular);
            prog->SetUniformVec3(position, obj->GetPosition());
            prog->SetUniform1f(constant, light->constant);
            prog->SetUniform1f(linear, light->linear);
            prog->SetUniform1f(quadratic, light->quadratic);
            point_lights++;
            break;
        }
        default:
            logger.Log(LOGTYPE::ERROR, "Renderer::CalculateAndSetLightingUniforms() --> got an object of non lighting type.\n"); break;
    }

}

TextRenderer::TextRenderer()
{
    if (!BuildTextShader())
        return;
    if (FT_Init_FreeType(&m_FT))
    {
        logger.Log(LOGTYPE::ERROR, "ERROR::FREETYPE: Could not init FreeType Library");
        return;
    }
    std::string font_path = util::getcwd() + "/rsrc/fonts/Ubuntu-Th.ttf";
    if (FT_New_Face(m_FT, font_path.c_str(), 0, &m_Face))
    {
        logger.Log(LOGTYPE::ERROR, "ERROR::FREETYPE: Failed to load font");  
        return;
    }
    FT_Set_Pixel_Sizes(m_Face, 0, 48);  
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(m_Face, c, FT_LOAD_RENDER))
        {
            logger.Log(LOGTYPE::ERROR, "ERROR::FREETYTPE: Failed to load Glyph");
            return;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            m_Face->glyph->bitmap.width,
            m_Face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            m_Face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture, 
            glm::ivec2(m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows),
            glm::ivec2(m_Face->glyph->bitmap_left,  m_Face->glyph->bitmap_top),
            static_cast<unsigned int>(m_Face->glyph->advance.x)
        };
        m_Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(m_Face);
    FT_Done_FreeType(m_FT);


    // configure VAO/VBO for texture quads
    glGenVertexArrays(1, &m_VA);
    util::checkGLError();
    glGenBuffers(1, &m_VB);
    util::checkGLError();
    glBindVertexArray(m_VA);
    util::checkGLError();
    glBindBuffer(GL_ARRAY_BUFFER, m_VB);
    util::checkGLError();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    util::checkGLError();
    glEnableVertexAttribArray(0);
    util::checkGLError();
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    util::checkGLError();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    util::checkGLError();
    glBindVertexArray(0);
    util::checkGLError();
    
}

TextRenderer::~TextRenderer()
{
    // need to implement
}

void TextRenderer::RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color)
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    m_TextShader->Bind();
    m_TextShader->SetUniformVec3("textColor", color);
    auto proj = gl.GetCamera()->GetOrthoProjectionMatrix();
    m_TextShader->SetUniformMat4("projection", proj);
    // m_TextShader->SetUniform1i("text", 0);
    glActiveTexture(GL_TEXTURE0);
    




    util::checkGLError();
    glBindVertexArray(m_VA);
    util::checkGLError();
    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = m_Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
       // printf("character %c\n", *c);
       // for (int i = 0; i < 6; i++)
       // {
       //     for (int j = 0; j < 4; j++)
       //         printf("%.2f,", vertices[i][j]);
       //     printf("\n");
       // }
        // render glyph texture over quad
        
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        util::checkGLError();
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, m_VB);
        util::checkGLError();
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
        util::checkGLError();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        util::checkGLError();
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        util::checkGLError();
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    

    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);

}

bool TextRenderer::BuildTextShader()
{
    Shader* vertex_shader = new Shader(util::getcwd() + "/src/shaders/text_vertex.glsl", GL_VERTEX_SHADER);
    if (vertex_shader->CheckError() != ShaderError::NO_ERROR_OK)
    {
        logger.Log(LOGTYPE::ERROR, vertex_shader->FetchLog());
        return false;
    }

    Shader* fragment_shader = new Shader(util::getcwd() + "/src/shaders/text_fragment.glsl", GL_FRAGMENT_SHADER);
    if (fragment_shader->CheckError() != ShaderError::NO_ERROR_OK)
    {
        logger.Log(LOGTYPE::ERROR, fragment_shader->FetchLog());
        return false;
    }
    m_TextShader = new ShaderProgram();
    m_TextShader->AddShader(vertex_shader);
    m_TextShader->AddShader(fragment_shader);

    if(!m_TextShader->Compile())
    {
        logger.Log(LOGTYPE::ERROR, "TextRenderer::BuildTextShader() --> unable to compile m_TextShader.\n");
        return false;
    }
    return true;
}
