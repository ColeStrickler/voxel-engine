#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <glbuffer.h>
#include <glvertexarray.h>
#include <renderobject.h>
#include "util.h"
#include "logger.h"

typedef struct 
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;
}SphereVertex;

class Sphere
{
public:
    Sphere(float radius, int resolution);
    ~Sphere();

    void addVertex(glm::vec3 pos, glm::vec3 normal, float phi, float theta);


    RenderObject* m_RenderObj;

private:
    int m_Resolution;
    float m_Radius;
    float m_StepValue;
    float m_HeightStep;

    std::vector<SphereVertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    ShaderProgram* m_SphereShader;
};


#endif