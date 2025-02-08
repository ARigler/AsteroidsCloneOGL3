#include "../include/Engine.h"
#include "../include/Game.h"
#include "../include/Utilities.h"
#include "../include/TextureManager.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include<gl/glew.h>

bool Engine::init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}
	}
	//use core OpenGL profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	//specify version 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	//Request a color buffer with 8-bits per RGBA channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	//Enable double-buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//Force OpenGL to use hardware acceleration
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	//Create window
	gWindow = SDL_CreateWindow("SDL2 Asteroids", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	if (gWindow == NULL)
	{
		printf("Window could not be created! %s\n", SDL_GetError());
		success = false;
	}
	else
	{	
		mContext = SDL_GL_CreateContext(gWindow);
		//init glew(experimental)
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			SDL_Log("Failed to initialize GLEW");
			success = false;
		}
		glGetError();//sometimes benign error code, so clear it.

		TextureManager* texMan = TextureManager::getInstance();
	}
	return success;
}

void Engine::close() {
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gRenderer = NULL;
	gWindow = NULL;

	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[]) {
	
	Engine* engine = Engine::getInstance();
	Game* game = Game::getInstance();
	game->runGame();

	no_dangle(game);
	no_dangle(engine);
	return 0;
}