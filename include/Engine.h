#pragma once
#ifndef RSOS_ENGINE
#define RSOS_ENGINE
#include<SDL.h>
#include<iostream>
#include "game.h";

class Engine {
public:
	static Engine* getInstance() {
		if (instance == nullptr) {
			instance = new Engine();
		}
		return instance;
	}
	//overload copy constructor and assignment operator so they can't mess with singleton pattern
	Engine(const Engine&) = delete;
	Engine operator=(const Engine&) = delete;
	~Engine() { close(); }
	SDL_Renderer* pass_renderer() { return gRenderer; }
	SDL_Window* pass_window() { return gWindow; }
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
private:
	bool init();
	void close();
	SDL_Window* gWindow;
	SDL_Renderer* gRenderer;
	SDL_GLContext mContext;
	static inline Engine* instance=nullptr;
	Engine(){
		if(!init())
		{
			printf("Engine failed to initialize!");
		}
	}
};

#endif