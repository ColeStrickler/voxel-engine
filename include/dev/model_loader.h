#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "logger.h"
#include "renderobject.h"
#include "texture.h"
#include "shader.h"
#include "renderer.h"

#define MODEL_REL_PATH "/include/dev/models/backpack/"


class ModelImporter
{
public:
    ModelImporter(ShaderProgram* sp);
    ~ModelImporter();

    void LoadModel(const std::string& path, unsigned int flags = aiProcess_Triangulate|aiProcess_FlipUVs);
    std::vector<RenderObject*>& GetObjects() {return m_MeshObjects;}

    uint32_t m_TotalVertices;
private:
    void ProcessNode(aiNode* node, const aiScene* scene);
    RenderObject* Mesh2Object(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture*> loadMaterialTextures(aiMaterial *mat, aiTextureType type, const std::string &typeName);
    std::vector<RenderObject*> m_MeshObjects;
    std::vector<Texture*> m_LoadedTextures;
    ShaderProgram* m_ShaderProgram;
};




#endif