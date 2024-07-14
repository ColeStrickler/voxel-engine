#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "logger.h"
#include "texture.h"
#include "shader.h"
#include "material.h"
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define POSITION_LOCATION 0
#define NORMAL_LOCATION 1
#define TEX_COORD_LOCATION 2
#define INVALID_MATERIAL 0xFFFFFFFF
enum MODELBUFFERTYPE
{
    INDEX_BUFFER = 0,
    VERTEX_BUFFER = 1,
    TEXCOORD_VB = 2,
    NORMAL_VB = 3,
    WVP_MAT_VB = 4,
    WORLD_MAT_VB = 5,
    NUM_BUFFERS = 6
};

struct Vector3f
{
    float x;
    float y;
    float z;
};
struct Vector2f
{
    float x;
    float y;
};
struct Vertex {
    Vector3f Position;
    Vector3f Normal;
    Vector2f TexCoords;
};
struct BasicMeshEntry {
    BasicMeshEntry()
    {
        NumIndices = 0;
        BaseVertex = 0;
        BaseIndex = 0;
        MaterialIndex = INVALID_MATERIAL;
    }
    uint32_t NumIndices;
    uint32_t BaseVertex;
    uint32_t BaseIndex;
    uint32_t MaterialIndex;
};

class MeshModel
{
public:
    MeshModel();
    ~MeshModel();
    void Render(LightingModel lModel, ShaderProgram* sp);
    void RenderPhong(ShaderProgram* sp);
    void HandlePhongShaders(int MeshIndex, int MaterialIndex, ShaderProgram* sp);
    unsigned int m_VAO;
    std::vector<BasicMeshEntry> m_Meshes;
    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
    std::vector<TextureObject> m_Materials;
    GLuint m_Buffers[NUM_BUFFERS] = { 0 };
};



class ModelImporter
{
public:
    ModelImporter(ShaderProgram* sp);
    ~ModelImporter();
    void Clear();
    void LoadModel(const std::string& path, unsigned int flags = aiProcess_Triangulate|aiProcess_FlipUVs|aiProcess_JoinIdenticalVertices|aiProcess_GenSmoothNormals);
    MeshModel* ExportCurrentModel();
    
    uint32_t m_TotalVertices;
    uint32_t m_TotalIndices;
private:
    void InitScene(const aiScene* scene);
    void CountVerticesAndIndices(const aiScene* scene);
    void LoadAllMeshes(const aiScene* scene);
    void InitMesh(uint32_t meshIndex, const aiMesh* mesh);
    void InitMaterials(const aiScene* scene);
    void LoadMaterialTextures(const aiMaterial* material, int index);
    void LoadDiffuseTexture(const aiMaterial* material, int index);
    void LoadDiffuseTextureEmbedded(const aiTexture* AiTexture, int index);
    void LoadDiffuseTextureFile(const aiMaterial* material, const std::string& file, int index);
    void LoadSpecularTexture(const aiMaterial* material, int index);
    void LoadSpecularTextureEmbedded(const aiTexture* AiTexture, int index);
    void LoadSpecularTextureFile(const aiMaterial* material, const std::string& file, int index);
    void PopulateBuffers();


    ShaderProgram* m_ShaderProgram;
    std::string m_CurrentDirectory;

    std::vector<BasicMeshEntry> m_Meshes;
    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
    std::vector<TextureObject> m_Materials;
    const  aiScene* m_Scene;
    GLuint m_Buffers[NUM_BUFFERS] = { 0 };
    unsigned int m_VAO;
};





#endif