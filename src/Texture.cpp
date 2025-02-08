#include "../include/Texture.h"
#include"../include/stb_image.h"
#include <GL/glew.h>
#include <SDL.h>

Texture::Texture()
	:mTextureID(0)
	, mWidth(0)
	, mHeight(0)
	, numChannels(0)
{

}

Texture::~Texture()
{

}

bool Texture::Load(const std::string& fileName)
{

	unsigned char *data = stbi_load(fileName.c_str(), &mWidth, &mHeight, &numChannels,0);

	if (data == nullptr)
	{
		SDL_Log("stb_i failed to load image %s", fileName.c_str());
		return false;
	}

	int format = GL_RGB;
	if (numChannels == 4)
	{
		format = GL_RGBA;
	}

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, format,GL_UNSIGNED_BYTE, data);
	//glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	// Enable bilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void Texture::Unload()
{
	glDeleteTextures(1, &mTextureID);
}

void Texture::SetActive()
{
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}