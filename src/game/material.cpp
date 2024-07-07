#include "material.h"
#include "logger.h"

extern Logger logger;


void TextureObject::Bind()
{
    if (DiffuseMap == nullptr)
        logger.Log(LOGTYPE::ERROR, "TextureObject::Bind() --> DiffuseMap was NULL.\n");
    if (SpecularMap == nullptr)
        logger.Log(LOGTYPE::ERROR, "TextureObject::Bind() --> SpecularMap was NULL.\n");
    DiffuseMap->SetTextureSlot(0);
    SpecularMap->SetTextureSlot(1);
    DiffuseMap->Bind();
    SpecularMap->Bind();
}