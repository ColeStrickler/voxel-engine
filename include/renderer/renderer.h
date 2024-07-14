#ifndef RENDERER_H
#define RENDERER_H
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "renderobject.h"
#include "gl.h"
#include "util.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#define MAX_POINT_LIGHTS 100
struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

class RenderObject;

class TextRenderer
{
public:
    TextRenderer();
    ~TextRenderer();
    void RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color);

    // move back to private
    ShaderProgram* m_TextShader;
private:
    bool BuildTextShader();

    unsigned int m_VA;
    unsigned int m_VB;

    std::unordered_map<char, Character> m_Characters;
    
    FT_Library m_FT;
    FT_Face m_Face;
};



class Renderer
{
public:
    Renderer();
    ~Renderer();
    void AddRenderObject(RenderObject* obj);
    void RenderAllObjects();
    void RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color){m_TextRenderer.RenderText(text,x,y,scale,color);};
    RenderObject* FindClosestObject(const glm::vec3& pos);
    void SetLightingModel(LightingModel model) {m_LightingModel = model;}
    LightingModel GetLightingModel() const {return m_LightingModel;}
    std::vector<RenderObject*> GetLighting() const {return m_LightingObjects;}


    // move back to private
    TextRenderer m_TextRenderer;
private:
    
    LightingModel m_LightingModel;
    std::vector<RenderObject*> m_RenderObjects;
    std::vector<RenderObject*> m_LightingObjects;
    //RenderObject* m_LightingObjects;
    std::mutex m_RenderLock;


    


    void HandleLightSources();
    void CalculateAndSetLightingUniforms(RenderObject* obj, int& point_lights);
};

#endif