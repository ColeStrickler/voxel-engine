#include "texture.h"
#include "stb_image.h"
#include <iostream>
#include "logger.h"


extern Logger logger;


Texture::Texture(const std::string& path, const std::string& name, unsigned int min_filter, 
	unsigned int mag_filter, unsigned int texture_wrap, unsigned int texture_storage_format,
	unsigned int texture_data_format) : m_FilePath(path), m_Name(name), m_LocalBuffer(nullptr), m_Height(0), 
	m_Width(0), m_RendererID(0), m_BPP(0), m_Slot(0)
{
	// Must flip image on load for use by OpenGL --> may need to play with this to get it right
	
	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_BPP, 0);
     if (!m_LocalBuffer)
    {
		logger.Log(LOGTYPE::ERROR, "Texture::Texture() Failed to load texture from file: " + path);
		size_t backSlashPos;
		while ((backSlashPos = m_FilePath.find('\\')) != std::string::npos)
		{
			logger.Log(LOGTYPE::INFO, "Texture::Texture() found backslashes in file path. Attempting to convert to Linux compatible path.");
			m_FilePath.replace(backSlashPos, 1, "/");
			std::cout << m_FilePath << std::endl;
			m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_BPP, 0);
			if (m_LocalBuffer != NULL)
			{
				logger.Log(LOGTYPE::INFO, "Texture::Texture() successfully recovered error.");
				break;
			}
		}
	}

	if (!m_LocalBuffer)
	{
		logger.Log(LOGTYPE::ERROR, "Texture::Texture() unable to recover error.");
		return;
	}
	logger.Log(LOGTYPE::INFO, "Texture::Texture() successfully loaded " + m_FilePath);
	glGenTextures(1, &m_RendererID);
	
	//glBindTexture(GL_TEXTURE_2D, 0);
	LoadInternal(m_LocalBuffer);

	if (m_LocalBuffer)
	{
		stbi_image_free(m_LocalBuffer);
	}
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_RendererID);
}

void Texture::SetTextureSlot(unsigned int slot = 0)
{
	m_Slot = slot;
}

void Texture::Bind() const
{
	glActiveTexture(GL_TEXTURE0 + m_Slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Load(uint32_t BufferSize, void *data)
{
	void* pImageData = stbi_load_from_memory((const stbi_uc*)data, BufferSize, &m_Width, &m_Height, &m_BPP, 0);


	stbi_image_free(pImageData);
}

void Texture::HandleFileTypeLoadSpecifics()
{
	std::string extension = ExtractFileExtension(m_FilePath);
	if (extension == "jpg")
		HandleLoadJPG();
	else if (extension == "png")
		HandleLoadPNG();
	else
		logger.Log(LOGTYPE::WARNING, "Texture::HandleFileTypeLoadSpecifics() --> no suitable load handler for extension " + extension);
}

void Texture::HandleLoadJPG()
{
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, m_LocalBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::HandleLoadPNG()
{
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::LoadInternal(void *pImageData)
{
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	printf("Texture Width: %d, Texture Height: %d\n", m_Width, m_Height);

    switch (m_BPP) {
        case 1:
		{
			
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_Width, m_Height, 0, GL_RED, GL_UNSIGNED_BYTE, pImageData);
            break;
		}
        case 2:
		{
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, m_Width, m_Height, 0, GL_RG, GL_UNSIGNED_BYTE, pImageData);
            break;
		}
        case 3:
        {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, pImageData);
            break;
		}
        case 4:
        {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImageData);
            break;
		}
        default:
            logger.Log(LOGTYPE::WARNING, "Texture::LoadInternal --> support for texture BPP " + std::to_string(m_BPP) + " not supported\n");
    }
	glGenerateMipmap(GL_TEXTURE_2D);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
    glBindTexture(GL_TEXTURE_2D, 0);
}
