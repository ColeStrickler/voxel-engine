#include "model_loader.h"

extern Logger logger;
extern Renderer renderer;

void ModelImporter::LoadModel(const std::string &path, unsigned int flags) 
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        logger.Log(LOGTYPE::ERROR, "ModelImporter::LoadModel::ERROR::ASSIMP " + std::string(import.GetErrorString()));
        return;
    }
    ProcessNode(scene->mRootNode, scene);
}

/*
    We may want to introduce a tree like hierarchy at some point here

    Would make traversing a hierarchical mesh easier
*/
ModelImporter::ModelImporter(ShaderProgram *sp) : m_ShaderProgram(sp), m_TotalVertices(0)
{
}
ModelImporter::~ModelImporter()
{
}
void ModelImporter::ProcessNode(aiNode *node, const aiScene *scene)
{
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
       // printf("processing mesh %d/%d\n", i, node->mNumMeshes);
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_MeshObjects.push_back(Mesh2Object(mesh, scene));
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

RenderObject *ModelImporter::Mesh2Object(aiMesh *mesh, const aiScene* scene)
{
    //printf("Mesh2Object()\n");
    // each vertex is assumed |vec3 position|vec3 normal|vec2 texture coords|
    size_t floats_per_vertex = 8;
    size_t vertex_size = sizeof(float) * floats_per_vertex;
    size_t vertices_size = mesh->mNumVertices * vertex_size;
    float *vertices = (float*)new char[vertices_size]; // 8 floats per vertex
    char* v = (char*)vertices;

    m_TotalVertices += mesh->mNumVertices;
    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
      //  printf("Mesh2Object() vertex %d/%d\n", i, mesh->mNumVertices);
        float buf[8];
        //printf("Mesh2Object() vertices\n");
        buf[0] = mesh->mVertices[i].x;
        buf[1] = mesh->mVertices[i].y;
        buf[2] = mesh->mVertices[i].z;
       // printf("Mesh2Object() normals\n");
        if (mesh->HasNormals())
        {
            buf[3] = mesh->mNormals[i].x;
            buf[4] = mesh->mNormals[i].y;
            buf[5] = mesh->mNormals[i].z;
        }
        
      //  printf("Mesh2Object() texture coords\n");
        if (mesh->mTextureCoords[i])
        {
            buf[6] = mesh->mTextureCoords[0][i].x;
            buf[7] = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            buf[6] = 0.0f;
            buf[7] = 0.0f;
        }
        memcpy(v, &buf[0], sizeof(buf));
        v += sizeof(buf);
    }
   // printf("Mesh2Object() finished vertex data\n");
    size_t uints_per_index = 3;
    size_t index_size = sizeof(unsigned int) * uints_per_index;
    size_t indices_size = mesh->mNumFaces * index_size;
    unsigned int* indices = (unsigned int *)new char[indices_size]; // 3 unsigned int indices per index --> see below
    char* ix = (char*)indices;
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        unsigned int buf[3];
        aiFace face = mesh->mFaces[i];

        /*
            we should be able to assume this is always 3 if we are using aiProcess_Triangulate flag
        */
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            buf[j] = face.mIndices[j];

        memcpy(ix, &buf[0], sizeof(buf));
        ix += sizeof(buf);
    }

    std::vector<Texture*> diff;
    std::vector<Texture*> spec;

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        diff = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        spec =loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    }

    VertexBuffer *vb = new VertexBuffer(vertices, vertices_size);
    IndexBuffer *ib = new IndexBuffer(indices, mesh->mNumFaces*3);
    VertexArray *va = new VertexArray();
    vb->SetLayout(new BufferLayout({new BufferElement("pos", ShaderDataType::Float3, false), new BufferElement("norm", ShaderDataType::Float3, false),\
    new BufferElement("tex", ShaderDataType::Float2, false)}));

    va->AddVertexBuffer(vb);
    va->AddIndexBuffer(ib);
    RenderObject *obj = new RenderObject(va, vb, m_ShaderProgram, ib, OBJECTYPE::ComplexModelObject);
    obj->m_TexturedObject.AddDiffuseMap(m_LoadedTextures[0]);
    obj->m_TexturedObject.AddSpecularMap(m_LoadedTextures[0]);
    obj->m_TexturedObject.Shininess = 0.01f;
    //delete vertices; --> need to track 
    //delete indices;
    return obj;
}

std::vector<Texture*>  ModelImporter::loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName)
{
    std::vector<Texture *> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        bool skip = false;
        mat->GetTexture(type, i, &str);

        std::string path(str.C_Str());
        for (auto &t : m_LoadedTextures)
        {
            if (t->m_FilePath.find(path) != std::string::npos)
            {
                textures.push_back(t);
                skip = true;
                break;
            }
        }
        if (skip)
            continue;
        Texture *tex = new Texture(util::getcwd() + MODEL_REL_PATH + str.C_Str(), typeName);
        m_LoadedTextures.push_back(tex);
        textures.push_back(tex);
        //printf("ModelImporter::loadMaterialTextures() successfully loaded %s\n", str.C_Str());
    }
    return textures;
}
