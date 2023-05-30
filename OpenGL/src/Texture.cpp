#include "Texture.h"
#include "stb_image/stb_image.h"

Texture::Texture(const std::string& path)
	: m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0) // BPP = bits per pixel
{
	stbi_set_flip_vertically_on_load(1); // flip the image vertically
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4); // 4 = RGBA
	GLCall(glGenTextures(1, &m_RendererID)); // generate 1 texture, store it in m_RendererID
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID)); // bind the texture to the renderer ID
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)); // set the texture parameter for minifying
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)); // set the texture parameter for magnifying
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); // set the texture parameter for wrapping
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)); // set the texture parameter for wrapping
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer)); // specify a 2D texture image
	GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // unbind the texture
	if (m_LocalBuffer) // if the local buffer exists
		stbi_image_free(m_LocalBuffer); // free the local buffer
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID)); // delete the texture
}

void Texture::Bind(unsigned int slot /*= 0*/) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // activate the texture slot
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID)); // bind the texture
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0)); // unbind the texture
}