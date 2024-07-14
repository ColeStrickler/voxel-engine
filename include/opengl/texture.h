#ifndef TEXTURE_H
#define TEXTURE_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <string>

static std::string ExtractFileExtension(const std::string& filename)
{
	    size_t dotIndex = filename.rfind('.');
		if (dotIndex != std::string::npos) {
			return filename.substr(dotIndex + 1);
		}
		return ""; // No extension found
}



class Texture
{
public:
	Texture() = default;
	Texture(const std::string& path, const std::string& name, unsigned int min_filter = GL_REPEAT, unsigned int mag_filter = GL_REPEAT,
		unsigned int texture_wrap = GL_CLAMP_TO_EDGE, unsigned int texture_storage_format = GL_RGB, 
		unsigned int texture_data_format = GL_RGB);
	~Texture();
	void SetTextureSlot(unsigned int slot);
	void Bind() const;
	void Unbind();

	void Load(uint32_t BufferSize, void* data);
	inline int GetWidth() const { return m_Width; };
	inline int GetHeight() const { return m_Height; };
	inline int GetTextureSlot() const { return m_Slot; };
	std::string GetName() const { return m_Name; };

	std::string m_FilePath;
private:
	void HandleFileTypeLoadSpecifics();
	void HandleLoadJPG();
	void HandleLoadPNG();
	void LoadInternal(void* pImageData);
	unsigned int m_RendererID;
	
	std::string m_Name;
	unsigned char* m_LocalBuffer;
	int m_Width;
	int m_Height;
	int m_BPP;
	int m_Slot;
};

#endif