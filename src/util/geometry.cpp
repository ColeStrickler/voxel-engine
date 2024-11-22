#include "geometry.h"
extern Logger logger;



Function3D::Function3D(float xRange, float yRange, float zRange, int resolution, ShaderProgram *sp) : m_xRange(xRange), m_yRange(yRange), m_zRange(zRange)
{
    m_HeightStep = (zRange*2.0f) / resolution;
    m_StepValue = (xRange*2.0f) / resolution;
    m_Resolution = resolution;



    int row = 0;
    int numVertices = 0;
    
    for (float z = -1*m_zRange; z <= m_zRange; z += m_HeightStep)
    {
        int cell = 0;
        
        for (float x = -1*m_xRange; x <= m_xRange; x += m_StepValue)
        {
            // substitute a equation calculator here
            // y = equation->eval(x, z);
            // for now y = x^2 + z^2
            float y = (x*x) + (z*z);

            // this may need to be reworked - may break things in how we push indices
            //if (y > m_yRange)
            //    continue;


            // gradient = normal 
            //printf("xInit = %.3f\n", xval);
            auto normal = glm::normalize(glm::vec3(-2*x, 1, -2*z));
            addVertex(glm::vec3(x, y, z), normal);
            //printf("loc: %.3f,%.3f,%.3f\tnormal %.3f,%.3f,%.3f\n",x,y,z,normal.x,normal.y,normal.z);
            if (row)
            {
                int nextCell = (cell+1);
                if (nextCell >= m_Resolution)
                    nextCell = cell;

                m_Indices.push_back(numVertices - m_Resolution); // bottom left
                m_Indices.push_back((row-1)*m_Resolution + nextCell); // bottom right
                m_Indices.push_back(row*m_Resolution + nextCell); // top right
                

                m_Indices.push_back(numVertices - m_Resolution); // bottom left
                m_Indices.push_back(numVertices); // top left(this vertex)
                m_Indices.push_back(row*m_Resolution+nextCell); // top right
            }

            numVertices++;
            cell++;
        }
        row++;
    }
    VertexBuffer* vb = new VertexBuffer((float*)m_Vertices.data(), m_Vertices.size() * sizeof(SphereVertex));
    BufferLayout* funcVBLayout = new BufferLayout({new BufferElement("COORDS", ShaderDataType::Float3, false),
         new BufferElement("NORMALS", ShaderDataType::Float3, false),
          new BufferElement("TEXCOORDS", ShaderDataType::Float2, false) });
    vb->SetLayout(funcVBLayout);
    IndexBuffer* ib = new IndexBuffer(m_Indices.data(), m_Indices.size());
    VertexArray* va = new VertexArray();
     
    va->AddVertexBuffer(vb);
    va->AddIndexBuffer(ib);
   
    va->SetCount(m_Indices.size());

   
    //Shader* sphereVertexShader = new Shader(util::getcwd() + "/src/shaders/3D_Geometry/sphereVertex.glsl", GL_VERTEX_SHADER)
    //Shader* sphereFragmentShader = new Shader(util::getcwd() + "/src/shaders/3D_Geometry/sphereFragment.glsl", GL_FRAGMENT_SHADER);
    

    m_FuncShader = sp;

    //if (!m_SphereShader->Compile())
    //{
    //    logger.Log(LOGTYPE::ERROR, "Sphere::Sphere --> Unable to compile Sphere Shader.");
    //    logger.Log(LOGTYPE::ERROR, "Error #" + std::to_string(m_SphereShader->CheckError()));
    //    while (1)
    //        ;
    //}

    m_RenderObj = new RenderObject(va, vb, m_FuncShader, ib, OBJECTYPE::RegularMaterial);
    m_RenderObj->m_MaterialId = PHONG_MATERIAL::BRONZE;
}

void Function3D::addVertex(glm::vec3 pos, glm::vec3 normal)
{
    glm::vec2 TexCoord;
    TexCoord.x = (pos.x + m_xRange) / (m_xRange*2);
    TexCoord.y = (pos.z + m_zRange) / (m_zRange*2);
    m_Vertices.push_back({pos, normal, TexCoord});
}



Sphere::Sphere(float sphereRadius, int resolution, ShaderProgram* sp) : m_Resolution(resolution), m_Radius(sphereRadius)
{
    //angle step between vertices 
    m_StepValue = glm::two_pi<float>() / static_cast<float>(m_Resolution);

    m_HeightStep = glm::pi<float>() / static_cast<float>(m_Resolution);
    //printf("radius %.3f\n", m_Radius);

    int row = 0;
    float phi = -glm::half_pi<float>(); // start at bottom of sphere
    float y = m_Radius*glm::sin(phi); // start at y = -1;
    float radius = glm::cos(phi);
    int numVertices = 0;

    for (; phi < glm::half_pi<float>() + m_HeightStep;)
    {
        y = m_Radius*glm::sin(phi);
       // printf("y %.3f\n", y);
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
    

    m_SphereShader = sp;

    //if (!m_SphereShader->Compile())
    //{
    //    logger.Log(LOGTYPE::ERROR, "Sphere::Sphere --> Unable to compile Sphere Shader.");
    //    logger.Log(LOGTYPE::ERROR, "Error #" + std::to_string(m_SphereShader->CheckError()));
    //    while (1)
    //        ;
    //}

    m_RenderObj = new RenderObject(va, vb, m_SphereShader, ib, OBJECTYPE::RegularMaterial);
    m_RenderObj->m_MaterialId = PHONG_MATERIAL::EMERALD;
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
