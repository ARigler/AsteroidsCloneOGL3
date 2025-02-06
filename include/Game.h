#pragma once
#ifndef RSOS_GAME
#define RSOS_GAME
#include<SDL_ttf.h>
#include<vector>
#include<map>
#include<iostream>
#include"Actor.h"
#include"Component.h"
#include"TextureManager.h"
#include"Engine.h"
#include<functional>

class Game {
public:
	static Game* getInstance() {
		if (instance == nullptr) {
			instance = new Game();
		}
		return instance;
	}
	//overload copy constructor and assignment operator so they can't mess with singleton pattern
	Game(const Game&) = delete;
	Game operator=(const Game&) = delete;
	~Game();
	bool init();
	void close();
	void runGame();
	const int FRAME_CAP=60;
	void add_actor(class Actor* actor);
	void remove_actor(class Actor* actor);
	int get_score()const { return mScore; }
	void add_score(int scoreArg);
	void reset_score() { mScore = 0; }
	void start_gameOverTimer() { gameOverTimer.start(); }
	void stop_gameOverTimer() { gameOverTimer.stop(); }
	Uint32 tick_gameOverTimer() { return gameOverTimer.getTicks(); }
	std::vector<class Actor*> getActors() { return mActors; }
private:
	void runFrame(float deltaTime);
	void processInput();
	void update(float deltaTime);
	void render();
	bool loadMedia();
	bool quit;
	int mScore;
	bool mUpdatingActors;
	TTF_Font* gFont;
	RSOS_Timer gameOverTimer;
	std::vector<class Actor*> mActors;
	std::vector<class Actor*> mPendingActors;
	std::vector<class Actor*> mDeadActors;
	std::multimap<int, class SpriteComponent*> mRenderLookupTable;
	std::multimap<int, std::function<void(float)>> mUpdateLookupTable;
	static inline Game* instance = nullptr;
	Game() {
		if (!init())
		{
			printf("Game failed to initialize!");
		}
	}
};

#endif