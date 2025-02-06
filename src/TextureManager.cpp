#include"../include/TextureManager.h"
#include"Utilities.h"
#include<SDL.h>
#include<SDL_image.h>
#include<iostream>
#include<SDL_ttf.h>

void TextureManager::setColor(int index, Uint8 red, Uint8 green, Uint8 blue)
{	TextureMetadata mTextureMetadata = fetchData(index);

	mTextureMetadata.colorMod = { red, green, blue };
	setData(index, mTextureMetadata);
}

void TextureManager::setBlendMode(int index, SDL_BlendMode blending) {
	TextureMetadata mTextureMetadata = fetchData(index);

	mTextureMetadata.blendMode = blending;
	setData(index, mTextureMetadata);

}

void TextureManager::setAlpha(int index, Uint8 alpha) {
	TextureMetadata mTextureMetadata = fetchData(index);

	mTextureMetadata.alpha=alpha;
	setData(index, mTextureMetadata);
}

void TextureManager::render(SDL_Renderer* renderer, int index, int x, int y, Uint8 alpha, ColorMod color, SDL_BlendMode blending, float scale, double angle, SDL_Rect* clip, SDL_Point* center, SDL_RendererFlip flip)
{
	if(index < textureList.size() && index >= 0) {
		SDL_Texture* mTexture = textureList[index].texture;
		TextureMetadata mTextureMetadata = fetchData(index);

		//if non-default args passed, apply them, otherwise fallback on metadata
		if (alpha != 0xFF) {
			SDL_SetTextureAlphaMod(mTexture, alpha);
		}
		else {
			SDL_SetTextureAlphaMod(mTexture, mTextureMetadata.alpha);
		}

		if (color.red != 0xFF || color.green != 0xFF || color.blue != 0xFF) {
			SDL_SetTextureColorMod(mTexture, color.red, color.green, color.blue);
		}
		else {
			SDL_SetTextureColorMod(mTexture, mTextureMetadata.colorMod.red, mTextureMetadata.colorMod.green, mTextureMetadata.colorMod.blue);
		}

		if (blending != SDL_BLENDMODE_BLEND) {
			SDL_SetTextureBlendMode(mTexture, blending);
		}
		else {
			SDL_SetTextureBlendMode(mTexture, mTextureMetadata.blendMode);
		}

		//Set rendering space and render to screen
		SDL_Rect renderQuad = { x, y, mTextureMetadata.width*scale, mTextureMetadata.height * scale };

		//Set clip rendering dimensions
		if (clip != NULL)
		{
			renderQuad.w = clip->w * scale;
			renderQuad.h = clip->h * scale;
		}

		//Render to screen
		SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip);

		mTexture = nullptr;
	}
	else {
		SDL_Log("Index %s does not exist in the texture database.",std::to_string(index).c_str());
	}
}

void TextureManager::render(SDL_Renderer* renderer, std::string text, int x, int y, Uint8 alpha, ColorMod color, SDL_BlendMode blending, float scale, double angle, SDL_Rect* clip, SDL_Point* center, SDL_RendererFlip flip)
{
	if (textTextureCache.find(text) != textTextureCache.end()) {

		SDL_Texture* mTexture = textTextureCache[text].text;
		TextMetadata mTextMetadata = fetchTextData(text);

		//if non-default args passed, apply them, otherwise fallback on metadata
		SDL_SetTextureAlphaMod(mTexture, alpha);

		if (color.red != 0xFF || color.green != 0xFF || color.blue != 0xFF) {
			SDL_SetTextureColorMod(mTexture, color.red, color.green, color.blue);
		}
		else {
			SDL_SetTextureColorMod(mTexture, mTextMetadata.textColor.r, mTextMetadata.textColor.g, mTextMetadata.textColor.b);
		}
		SDL_SetTextureBlendMode(mTexture, blending);

		//Set rendering space and render to screen
		SDL_Rect renderQuad = { x, y, mTextMetadata.width, mTextMetadata.height };

		//Set clip rendering dimensions
		if (clip != NULL)
		{
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}

		//Render to screen
		SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip);

		mTexture = nullptr;
	}
	else {
		SDL_Log("Text %s does not exist in the texture database.", text.c_str());
	}
}

void TextureManager::render(SDL_Renderer* renderer, SDL_Texture* mTexture, int x, int y, Uint8 alpha, ColorMod color, SDL_BlendMode blending, float scale, double angle, SDL_Rect* clip, SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Point size;
	SDL_QueryTexture(mTexture, NULL, NULL, &size.x, &size.y);
		
	//if non-default args passed, apply them, otherwise fallback on metadata
	SDL_SetTextureAlphaMod(mTexture, alpha);

	SDL_SetTextureColorMod(mTexture, color.red, color.green, color.blue);
	SDL_SetTextureBlendMode(mTexture, blending);

	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, size.x, size.y };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	
	//Render to screen
	SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip);
	
	mTexture = nullptr;
}


void TextureManager::setData(int index, TextureMetadata metaData) {
	textureList[index].metaData = metaData;
}

TextureMetadata TextureManager::fetchData(int index) {
	if (index < textureList.size() && index>=0)
	{
		return textureList[index].metaData;
	}
}

TextMetadata TextureManager::fetchTextData(std::string text) {
	if (textTextureCache.find(text) != textTextureCache.end()) {
			return textTextureCache[text].metaData;
	}
}


bool TextureManager::loadFromFile(std::string path, SDL_Renderer* renderer,Uint8 alpha,ColorMod colorMod,SDL_BlendMode blending,ColorMod colorKey)
{
	//The final texture
	SDL_Texture* newTexture = NULL;
	int mWidth;
	int mHeight;
	int index;
	TextureMetadata newMetadata;
	bool success = true;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		SDL_Log("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		success = false;
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, colorKey.red, colorKey.green, colorKey.blue));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == NULL)
		{
			SDL_Log("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
			success = false;
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;

			//form new metadata
			index = textureList.size();
			newMetadata = { index,mWidth,mHeight,colorMod,blending,alpha };
			
			//push
			TextureData newData = { newTexture,newMetadata };
			textureList.push_back(newData);

			//Return success
			return textureList[index].texture != nullptr;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	return success;
}

bool TextureManager::loadFromRenderedText(std::string textInput, TTF_Font* gFont, SDL_Renderer* renderer, SDL_Color textColor)
{
	bool success = true;
	//if the string is a key in textTextureCache
	if (textTextureCache.find(textInput) != textTextureCache.end()){
		TextMetadata tempTextMetadata = textTextureCache[textInput].metaData;
		if (tempTextMetadata.textColor.r != textColor.r || tempTextMetadata.textColor.g != textColor.g || tempTextMetadata.textColor.b != textColor.b) {
			SDL_DestroyTexture(textTextureCache[textInput].text);
			textTextureCache.erase(textInput);
			return loadFromRenderedText(textInput, gFont, renderer, textColor);
		}
	}
	else {
		SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textInput.c_str(), textColor);
		if (textSurface == NULL)
		{
			SDL_Log("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
			success = false;
		}
		else {
			SDL_Texture* textTexture;
			textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
			TextMetadata textMetadata;
			if (textTexture == NULL)
			{
				SDL_Log("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else {
				textMetadata.width = textSurface->w;
				textMetadata.height = textSurface->h;
				TextData tData = { textTexture,textMetadata };
				textTextureCache.insert({ textInput, tData });
			};
		}
	}
	return success;
}

void TextureManager::removeIndex(int index) {
	//if the index is in the vector
	if (index < textureList.size())
	{
		SDL_DestroyTexture(textureList[index].texture);
		textureList.erase(textureList.begin() + index);
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

TextureManager::~TextureManager() {
	//deallocate every pointer in the vectors' lists and remove every entry
	for (int i = textureList.size()-1; i >= 0; i--) {
		removeIndex(i);
	}
	//deallocate every pointer in the unordered_map's lists and remove every entry
	for (auto& pair : textTextureCache) {
		SDL_DestroyTexture(pair.second.text);
	}
	textTextureCache.clear();
}