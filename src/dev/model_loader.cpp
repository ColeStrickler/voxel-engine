#include "model_loader.h"

extern Logger logger;


MeshModel::MeshModel()
{
}

MeshModel::~MeshModel()
{
}

void MeshModel::Render(LightingModel lModel, ShaderProgram* sp)
{
    switch (lModel)
    {
        case LightingModel::Phong:
        {
            RenderPhong(sp);
            break;
        }
        default:
            logger.Log(LOGTYPE::WARNING, "MeshModel::Render() --> LightingModel lModel is invalid.\n");
    }
}

void MeshModel::RenderPhong(ShaderProgram* sp)
{
    glBindVertexArray(m_VAO);
    for (unsigned int i = 0 ; i < m_Meshes.size() ; i++) {
        unsigned int MaterialIndex = m_Meshes[i].MaterialIndex;
        assert(MaterialIndex < m_Materials.size());
        HandlePhongShaders(i, MaterialIndex, sp);

        glDrawElementsBaseVertex(GL_TRIANGLES,
                                 m_Meshes[i].NumIndices,
                                 GL_UNSIGNED_INT,
                                 (void*)(sizeof(unsigned int) * m_Meshes[i].BaseIndex),
                                 m_Meshes[i].BaseVertex);
    }
    glBindVertexArray(0);
}

void MeshModel::HandlePhongShaders(int MeshIndex, int MaterialIndex, ShaderProgram* sp)
{
    
    
    
    
    auto diff = m_Materials[MaterialIndex].DiffuseMap;
    if (diff)
    {
        diff->SetTextureSlot(0);
        sp->SetUniform1i("textureObject.diffuseMap", diff->GetTextureSlot());
        diff->Bind();
        sp->SetUniform1i("textureObject.useDiffuse", 1);
    }
    else
        sp->SetUniform1i("textureObject.useDiffuse", 0);

    auto spec = m_Materials[MaterialIndex].SpecularMap;
    if (spec)
    {
        sp->SetUniform1i("textureObject.specularMap", spec->GetTextureSlot());
        spec->SetTextureSlot(1);
        spec->Bind();
        sp->SetUniform1i("textureObject.useSpecular", 1);
    }
    else
        sp->SetUniform1i("textureObject.useSpecular", 0);
}

void ModelImporter::LoadModel(const std::string &path, unsigned int flags) 
{

    m_CurrentDirectory = path.substr(0, path.find_last_of('/')) + "/";    


    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        logger.Log(LOGTYPE::ERROR, "ModelImporter::LoadModel::ERROR::ASSIMP " + std::string(importer.GetErrorString()));
        return;
    }
    InitScene(scene);
}

MeshModel* ModelImporter::ExportCurrentModel()
{
    MeshModel* model = new MeshModel();

    model->m_VAO = m_VAO;
    model->m_Vertices = m_Vertices;
    model->m_Indices = m_Indices;

    for (int i = 0; i < MODELBUFFERTYPE::NUM_BUFFERS; i++)
        model->m_Buffers[i] = m_Buffers[i];
    model->m_Materials = m_Materials;
    model->m_Meshes = m_Meshes;
    return model;
}

/*
    We may want to introduce a tree like hierarchy at some point here

    Would make traversing a hierarchical mesh easier
*/
ModelImporter::ModelImporter(ShaderProgram *sp) : m_ShaderProgram(sp), m_TotalVertices(0), m_TotalIndices(0)
{
}
ModelImporter::~ModelImporter()
{
}
void ModelImporter::InitScene(const aiScene *scene)
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    m_Meshes.resize(scene->mNumMeshes);
    m_Materials.resize(scene->mNumMaterials);
    m_Scene = scene;

    CountVerticesAndIndices(scene);
    LoadAllMeshes(scene);
    InitMaterials(scene);
    PopulateBuffers();
}
void ModelImporter::CountVerticesAndIndices(const aiScene *scene)
{
    for (unsigned int i = 0 ; i < m_Meshes.size() ; i++) {
        m_Meshes[i].MaterialIndex = scene->mMeshes[i]->mMaterialIndex;
        m_Meshes[i].NumIndices = scene->mMeshes[i]->mNumFaces * 3;
        m_Meshes[i].BaseVertex = m_TotalVertices;
        m_Meshes[i].BaseIndex = m_TotalIndices;

        m_TotalVertices += scene->mMeshes[i]->mNumVertices;
        m_TotalIndices  += m_Meshes[i].NumIndices;
    }

    m_Vertices.reserve(m_TotalVertices);
    m_Indices.reserve(m_TotalIndices);
}
void ModelImporter::LoadAllMeshes(const aiScene *scene)
{
    for (uint32_t i = 0; i < m_Meshes.size(); i++)
    {
        const aiMesh* mesh = scene->mMeshes[i];
        InitMesh(i, mesh);
    }
}
void ModelImporter::InitMesh(uint32_t meshIndex, const aiMesh *mesh)
{
    const aiVector3D zero(0.0f, 0.0f, 0.0f);
    Vertex v;
    
    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        auto& pos = mesh->mVertices[i];
        v.Position.x = pos.x;
        v.Position.y = pos.y;
        v.Position.z = pos.z;
        if (mesh->HasNormals())
        {
            auto& normal = mesh->mNormals[i];
            v.Normal.x = normal.x;
            v.Normal.y = normal.y;
            v.Normal.z = normal.z;
        }
        auto& texcoord =  mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : zero;
        v.TexCoords.x = texcoord.x;
        v.TexCoords.y = texcoord.y;
        m_Vertices.push_back(v);
    }

    // Populate the index buffer
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& Face = mesh->mFaces[i];
        m_Indices.push_back(Face.mIndices[0]);
        m_Indices.push_back(Face.mIndices[1]);
        m_Indices.push_back(Face.mIndices[2]);
    }
}

void ModelImporter::InitMaterials(const aiScene *scene)
{
    for (uint32_t i = 0; i < scene->mNumMaterials; i++)
    {
        const aiMaterial* mat = scene->mMaterials[i];
        LoadMaterialTextures(mat, i);
        // Load Colors() https://github.com/emeiri/ogldev/blob/master/Common/ogldev_basic_mesh.cpp#L576

    }
}
void ModelImporter::LoadMaterialTextures(const aiMaterial *material, int index)
{
    LoadDiffuseTexture(material, index);
    LoadSpecularTexture(material, index);
}

void ModelImporter::LoadDiffuseTexture(const aiMaterial *material, int index)
{   
    m_Materials[index].DiffuseMap = nullptr;
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    { 
        aiString Path;

        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
            const aiTexture* paiTexture = m_Scene->GetEmbeddedTexture(Path.C_Str());
            if (paiTexture) {
                LoadDiffuseTextureEmbedded(paiTexture, index);
            } else {
                LoadDiffuseTextureFile(material, Path.C_Str(), index);
            }
        }
    }
    else
        logger.Log(LOGTYPE::INFO, "ModelImporter::LoadDiffuseTexture() --> material " + std::to_string(index) + " has no diffuse texture.");
}

void ModelImporter::LoadDiffuseTextureEmbedded(const aiTexture* AiTexture, int index)
{
    m_Materials[index].DiffuseMap = new Texture();
    int buffer_size = AiTexture->mWidth;
    m_Materials[index].DiffuseMap->Load(buffer_size, AiTexture->pcData);
}

void ModelImporter::LoadDiffuseTextureFile(const aiMaterial *material, const std::string& file, int index)
{
    m_Materials[index].DiffuseMap = new Texture(m_CurrentDirectory + file, "diff");
}

void ModelImporter::LoadSpecularTexture(const aiMaterial *material, int index)
{  
    m_Materials[index].SpecularMap = nullptr;
    if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
    {
        aiString Path;

        if (material->GetTexture(aiTextureType_SHININESS, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
            const aiTexture* paiTexture = m_Scene->GetEmbeddedTexture(Path.C_Str());

            if (paiTexture) {
                LoadSpecularTextureEmbedded(paiTexture, index);
            } else {
                LoadSpecularTextureFile(material, Path.C_Str(), index);
            }
        }
    }
}

void ModelImporter::LoadSpecularTextureEmbedded(const aiTexture *AiTexture, int index)
{
    m_Materials[index].SpecularMap = new Texture();
    int buffer_size = AiTexture->mWidth;
    m_Materials[index].SpecularMap->Load(buffer_size, AiTexture->pcData);
}

void ModelImporter::LoadSpecularTextureFile(const aiMaterial *material, const std::string &file, int index)
{
    m_Materials[index].SpecularMap = new Texture(m_CurrentDirectory + file, "spec");
}

void ModelImporter::PopulateBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VERTEX_BUFFER]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices[0]) * m_Vertices.size(), &m_Vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);
    
    size_t NumFloats = 0;

    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(NumFloats * sizeof(float)));
    NumFloats += 3;

    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(NumFloats * sizeof(float)));
    NumFloats += 3;

    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(NumFloats * sizeof(float)));
    

    
}
