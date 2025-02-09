#include"../include/TextureManager.h"
#include"../include/Utilities.h"
#include<SDL.h>
#include<SDL_image.h>
#include<iostream>
#include<SDL_ttf.h>


Shader* TextureManager::getShader(int index) {
	if (index < shaderList.size() && index >= 0) {
		return shaderList[index];
	}
}

Texture* TextureManager::getTexture(int index) {
	if (index < textureList.size() && index >= 0) {
		return textureList[index];
	}
}

VertexArray* TextureManager::getVA(int index) {
	if (index < vertexArrayList.size() && index >= 0) {
		return vertexArrayList[index];
	}
}

bool TextureManager::loadTexture(std::string path) {
	Texture* texture = new Texture();
	texture->Load(path);
	textureList.push_back(texture);
	return true;
}

bool TextureManager::loadShaders(std::string vertPath, std::string fragPath) {
	Shader* shader = new Shader();
	if (!shader->Load(vertPath, fragPath)) {
		return false;
	}
	shader->SetActive();
	//set the view-projection matrix
	Matrix4 viewProj = Matrix4::CreateSimpleViewProj(800.f, 600.f);
	shader->SetMatrixUniform("uViewProj", viewProj);
	shaderList.push_back(shader);
	shader->SetActive();
}

void TextureManager::setActiveShader(int index) {
	if (index < shaderList.size() && index >= 0) {
		shaderList[index]->SetActive();
	}
}

void TextureManager::setActiveVertexArray(int index) {
	if (index < vertexArrayList.size() && index >= 0) {
		vertexArrayList[index]->setActive();
	}
}

void TextureManager::removeIndex(int index) {
	//if the index is in the vector
	if (index < textureList.size()&&index>=0)
	{
		textureList[index]->Unload();
		delete textureList[index];
		textureList.erase(textureList.begin() + index);
	}
}

void TextureManager::removeShader(int index) {
	//if the index is in the vector
	if (index < shaderList.size() && index >= 0)
	{
		delete shaderList[index];
		shaderList.erase(shaderList.begin() + index);
	}
}

void TextureManager::removeVertexArray(int index) {
	//if the index is in the vector
	if (index < vertexArrayList.size() && index >= 0)
	{
		delete vertexArrayList[index];
		vertexArrayList.erase(vertexArrayList.begin() + index);
	}
}

void TextureManager::removeText(std::string textInput) {
	//if the string is in the map
	if(textTextureCache.find(textInput) != textTextureCache.end()) 
	{
		SDL_DestroyTexture(textTextureCache[textInput].text);
		textTextureCache.erase(textInput);
	}
}

void TextureManager::createSpriteVerts() {
	float vertices[] = {
		-0.5f,0.5f,0.f,0.f,0.f,
		0.5f,0.5f,0.f,1.f,0.f,
		0.5f,-0.5f,0.f,1.f,1.f,
		-0.5f,-0.5f,0.f,0.f,1.f,
	};

	unsigned int indices[] = {
		0,1,2,
		2,3,0
	};
	vertexArrayList.push_back(new VertexArray(vertices, 4, indices, 6));
}

TextureManager::~TextureManager() {
	//deallocate every pointer in the vectors' lists and remove every entry
	for (int i = textureList.size()-1; i >= 0; i--) {
		removeIndex(i);
	}
	//deallocate every shader
	for (int i = shaderList.size() - 1; i >= 0; i--) {
		shaderList[i]->Unload();
		removeShader(i);
	}
	//deallocate every vertex array
	for (int i = vertexArrayList.size() - 1; i >= 0; i--) {
		removeVertexArray(i);
	}
	//deallocate every pointer in the unordered_map's lists and remove every entry
	for (auto& pair : textTextureCache) {
		SDL_DestroyTexture(pair.second.text);
	}
	textTextureCache.clear();
}