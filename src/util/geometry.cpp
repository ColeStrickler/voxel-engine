#include "geometry.h"
extern Logger logger;

Sphere::Sphere(float sphereRadius, int resolution) : m_Resolution(resolution), m_Radius(sphereRadius)
{
    //angle step between vertices 
    m_StepValue = glm::two_pi<float>() / static_cast<float>(m_Resolution);

    m_HeightStep = glm::pi<float>() / static_cast<float>(m_Resolution);
    printf("radius %.3f\n", m_Radius);

    int row = 0;
    float phi = -glm::half_pi<float>(); // start at bottom of sphere
    float y = m_Radius*glm::sin(phi); // start at y = -1;
    float radius = glm::cos(phi);
    int numVertices = 0;

    for (; phi < glm::half_pi<float>() + m_HeightStep;)
    {
        y = m_Radius*glm::sin(phi);
        printf("y %.3f\n", y);
        radius = m_Radius * glm::cos(phi);
        
        int cell = 0;
        for (float theta = 0.0f; theta < glm::two_pi<float>(); )
        {
            glm::vec3 normalVector = glm::normalize(glm::vec3(radius*glm::cos(theta), y, radius*glm::sin(theta)));
            //printf("x %.3f\tz %.3f\ty%.3f\n", radius*glm::cos(theta), y, radius*glm::sin(theta));


            addVertex(glm::vec3(radius*glm::cos(theta), y, radius*glm::sin(theta)), normalVector, phi, theta);
            
            if (row)
            {
                int nextCell = (cell+1) % m_Resolution;
                m_Indices.push_back(numVertices - m_Resolution); // bottom left
                m_Indices.push_back((row-1)*m_Resolution + nextCell); // bottom right
                m_Indices.push_back(row*m_Resolution + nextCell); // top right
                

                m_Indices.push_back(numVertices - m_Resolution); // bottom left
                m_Indices.push_back(numVertices); // top left(this vertex)
                m_Indices.push_back(row*m_Resolution+nextCell); // top right
            }
            

            numVertices++;
            theta += m_StepValue; cell++;
        }
         phi += m_HeightStep; row++;
        // printf("phi %.3f\n", phi);
    }
    
    VertexBuffer* vb = new VertexBuffer((float*)m_Vertices.data(), m_Vertices.size() * sizeof(SphereVertex));
    BufferLayout* sphereVBLayout = new BufferLayout({new BufferElement("COORDS", ShaderDataType::Float3, false),
         new BufferElement("NORMALS", ShaderDataType::Float3, false),
          new BufferElement("TEXCOORDS", ShaderDataType::Float2, false) });
    vb->SetLayout(sphereVBLayout);
    IndexBuffer* ib = new IndexBuffer(m_Indices.data(), m_Indices.size());
    VertexArray* va = new VertexArray();
     
    va->AddVertexBuffer(vb);
    va->AddIndexBuffer(ib);
   
    va->SetCount(m_Indices.size());

   
    //Shader* sphereVertexShader = new Shader(util::getcwd() + "/src/shaders/3D_Geometry/sphereVertex.glsl", GL_VERTEX_SHADER)
    //Shader* sphereFragmentShader = new Shader(util::getcwd() + "/src/shaders/3D_Geometry/sphereFragment.glsl", GL_FRAGMENT_SHADER);
    
    Shader* sphereVertexShader = new Shader(util::getcwd() + "/src/shaders/vertex.glsl", GL_VERTEX_SHADER);
    Shader* sphereFragmentShader = new Shader(util::getcwd() + "/src/shaders/fragment.glsl", GL_FRAGMENT_SHADER);
    m_SphereShader = new ShaderProgram();
    m_SphereShader->AddShader(sphereVertexShader);
    m_SphereShader->AddShader(sphereFragmentShader);

    if (!m_SphereShader->Compile())
    {
        logger.Log(LOGTYPE::ERROR, "Sphere::Sphere --> Unable to compile Sphere Shader.");
        logger.Log(LOGTYPE::ERROR, "Error #" + std::to_string(m_SphereShader->CheckError()));
        while (1)
            ;
    }

    m_RenderObj = new RenderObject(va, vb, m_SphereShader, ib, OBJECTYPE::RegularMaterial);
    m_RenderObj->m_Material = materials[PHONG_MATERIAL::COPPER];

}

Sphere::~Sphere()
{
}

void Sphere::addVertex(glm::vec3 pos, glm::vec3 normal, float phi, float theta)
{
    glm::vec2 texCoord;
    texCoord.x = theta / glm::two_pi<float>(); // 2pi is x range
    texCoord.y = (phi + glm::half_pi<float>()) / glm::pi<float>(); // pi is y range

    m_Vertices.push_back({pos,normal, texCoord});
}
