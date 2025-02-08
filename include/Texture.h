#ifndef RSOS_TEXTURE
#define RSOS_TEXTURE
#include <string>


class Texture
{
public:
	Texture();
	~Texture();

	bool Load(const std::string& fileName);
	void Unload();

	void SetActive();

	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
	int GetChannels() const { return numChannels; }
private:
	// OpenGL ID of this texture
	unsigned int mTextureID;
	// Width/height of the texture
	int mWidth;
	int mHeight;
	int numChannels;

};

#endif