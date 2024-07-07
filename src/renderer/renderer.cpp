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
        default:
            logger.Log(LOGTYPE::WARNING, "Renderer::AddRenderObject() --> attempted to add RenderObject with invalid OBJECTTYPE.\n");
    }
    

}

void Renderer::RenderAllObjects()
{
    std::unique_lock lock(m_RenderLock);

    HandleLightSources();
    

    int i = m_RenderObjects.size();
    for (int x = 0; x < i;  x++)
    {
        auto& obj = m_RenderObjects[x];
        obj->Render();
    }
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
