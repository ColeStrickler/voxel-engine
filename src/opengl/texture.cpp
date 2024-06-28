#include "texture.h"
#include "stb_image.h"
#include <iostream>

Texture::Texture(const std::string& path, const std::string& name, unsigned int min_filter, 
	unsigned int mag_filter, unsigned int texture_wrap, unsigned int texture_storage_format,
	unsigned int texture_data_format) : m_FilePath(path), m_Name(name), m_LocalBuffer(nullptr), m_Height(0), 
	m_Width(0), m_RendererID(0), m_BPP(0), m_Slot(0)
{
	// Must flip image on load for use by OpenGL --> may need to play with this to get it right
	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 0);
    if (!m_LocalBuffer)
        std::cout << "Failed to load texture" << std::endl;
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, m_LocalBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);


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