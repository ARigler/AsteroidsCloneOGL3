#pragma once
#ifndef RSOS_TEXTUREMANAGER
#define RSOS_TEXTUREMANAGER
#include<vector>
#include<SDL.h>
#include<SDL_ttf.h>
#include<string>
#include<unordered_map>
#include"Structures.h"
#include"Shader.h"
#include"VertexArray.h"
#include"Texture.h"

struct TextMetadata {
	int width;
	int height;
	SDL_Color textColor;
};

struct TextData {
	SDL_Texture* text;
	TextMetadata metaData;
};

struct TilesetMetadata {
	int resourceIndex;
	int cellWidth;
	int cellHeight;
};

class TextureManager {
public:
	static TextureManager* getInstance() {
		if (instance == nullptr) {
			instance = new TextureManager();
		}
		return instance;
	}
	TextureManager(const TextureManager&) = delete;
	TextureManager operator=(const TextureManager&) = delete;
	//Renders texture at given point
	bool loadShaders(std::string vertPath, std::string fragPath);
	bool loadTexture(std::string path);
	void setActiveShader(int index);
	void setActiveVertexArray(int index);
	void createSpriteVerts();
	Shader* getShader(int index);
	Texture* getTexture(int index);
	VertexArray* getVA(int index);
	void removeIndex(int index);
	void removeShader(int index);
	void removeVertexArray(int index);
	void removeText(std::string textInput);
	int fetchTextureListLength()const { return textureList.size(); }
	~TextureManager();
private:
	std::vector<Texture*> textureList;
	std::unordered_map<std::string, TextData> textTextureCache;
	std::vector<Shader*> shaderList;
	std::vector<VertexArray*> vertexArrayList;
	TextureManager() {
		textureList = std::vector<Texture*>();
		textTextureCache = std::unordered_map<std::string, TextData>();
		shaderList = std::vector<Shader*>();
		vertexArrayList = std::vector<VertexArray*>();
		loadShaders("shaders/Sprite.vert", "shaders/Sprite.frag");
		createSpriteVerts();
		setActiveShader(0);
		setActiveVertexArray(0);
	}
	inline static TextureManager* instance=nullptr;
};

#endif