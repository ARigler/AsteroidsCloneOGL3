#include "../include/Engine.h"
#include"../include/Game.h"
#include"../include/Timer.h"
#include"../include/TextureManager.h"
#include"../include/AudioManager.h"
#include"../include/Component.h"
#include"../include/Random.h"
#include"Math.h"
#include<iostream>
#include<string>
#include<algorithm>
#include<SDL.h>
#include<SDL_mixer.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<map>
#include<functional>


Game::~Game() {
	close();
}


void Game::add_actor(Actor* actor) {
	if (mUpdatingActors) {
		mPendingActors.emplace_back(actor);
	}
	else {
		mActors.emplace_back(actor);
	}
}

void Game::add_score(int scoreArg){
	mScore += scoreArg;
	TextureManager* texMan = TextureManager::getInstance();
	Engine* engine = Engine::getInstance();
	texMan->loadFromRenderedText(std::to_string(mScore), gFont, engine->pass_renderer());
}


void Game::remove_actor(Actor* actor) {
		auto actorToRemove = std::find(mActors.begin(), mActors.end(), actor);
		if (actorToRemove != mActors.end())
		{
			std::iter_swap(actorToRemove, mActors.end() - 1);
			mActors.pop_back();
		}
}

bool Game::loadMedia() {
	bool success = true;

	gFont = TTF_OpenFont("assets/lucon.ttf", 28);
	if (gFont == NULL) {
		SDL_Log("Failed to load lucida console font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	SDL_Renderer* renderer = Engine::getInstance()->pass_renderer();
	TextureManager* texMan = TextureManager::getInstance();
	AudioManager* audMan = AudioManager::getInstance();
	texMan->loadFromFile("assets/Ship 3.png", renderer);
	texMan->loadFromFile("assets/Space.png", renderer);
	texMan->loadFromFile("assets/A1.png", renderer);
	texMan->loadFromFile("assets/A2.png", renderer);
	texMan->loadFromFile("assets/P7.png", renderer);
	texMan->loadFromFile("assets/M1.png", renderer);
	texMan->loadFromFile("assets/M4.png", renderer);
	texMan->loadFromFile("assets/P9.png", renderer);
	texMan->loadFromFile("assets/P10.png", renderer);

	texMan->loadFromRenderedText("Score: ",gFont,renderer);
	texMan->loadFromRenderedText(std::to_string(mScore), gFont, renderer);

	audMan->loadClipFromFile("assets/151021__bubaproducer__laser-shot-big-3.wav");
	audMan->loadClipFromFile("assets/446624__idkmrgarcia__explosion2.wav");

	renderer = nullptr;

	return success;
}

bool Game::init() {
	bool success = true;
	quit = false;
	mUpdatingActors = false;
	mScore = 0;
	SDL_Log("Initializing");

	if (!loadMedia()) {
		SDL_Log("Loading assets failed!");
		success = false;
	}

	Random::Init();

	mActors = std::vector<Actor*>();
	mPendingActors = std::vector<Actor*>();
	mDeadActors = std::vector<Actor*>();
	mRenderLookupTable = std::multimap<int, SpriteComponent*>();

	Ship* ship = new Ship(this, Vector2(Engine::getInstance()->SCREEN_WIDTH / 2.0f, Engine::getInstance()->SCREEN_HEIGHT / 2.0f), 1.0f);
	WarpZone* warp = new WarpZone(this);
	add_actor(ship);
	const int numAsteroids = 20;
	for (int i = 0; i < numAsteroids; i++) {
		Asteroid* ast = new Asteroid(this);
		add_actor(ast);
	}
	add_actor(warp);

	return success;
}

void Game::close() {
	while (!mActors.empty())
	{
		delete mActors.back();
		mActors.pop_back();
	}

	gFont = NULL;

}

void Game::processInput() {
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		quit = false;
	}

	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->processInput(keyState);
	}
	mUpdatingActors = false;
}


void Game::update(float deltaTime) {
	mUpdatingActors = true;
	//event timers
	if (tick_gameOverTimer() > 150) {
		stop_gameOverTimer();
		reset_score();
		Ship* ship = new Ship(this, Vector2(Engine::getInstance()->SCREEN_WIDTH / 2.0f, Engine::getInstance()->SCREEN_HEIGHT / 2.0f), 1.0f);
		add_actor(ship);
	}


	//populate update functions in a multimap based on update order
	for (Actor* actor : mActors) {
		//update actor and then populate the lookup table with components' update order and functions.
		actor->update(deltaTime);
		for (Component* component : actor->getComponents()) {
			mUpdateLookupTable.insert({ component->get_uO(), std::bind(&Component::update, component, deltaTime) });
		}

		//execute update functions based on update order (ascending)
		for (auto element : mUpdateLookupTable) {
			element.second(deltaTime);
		}
		mUpdateLookupTable.clear();
	}

	mUpdatingActors = false;
	for (auto pending : mPendingActors) {
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();
		
	mDeadActors = std::vector<class Actor*>();
	//construct deadActors
	for (auto actor : mActors) {
		ActorType actorLog = actor->get_aType();
		if (actor->get_state() == Actor::EDead) {
			mDeadActors.push_back(actor);
		}
	}
	//remove actors that were put in deadActors.
	mActors.erase(
		std::remove_if(mActors.begin(), mActors.end(), [](Actor* actor) {
			return actor->get_state() == Actor::EDead;
			}),
		mActors.end()
	);

	//free and delete mDeadActors entries
	for (auto actor: mDeadActors) {
		ActorType actorLog;
		actorLog = actor->get_aType();
		delete actor;
	}
	mDeadActors.clear();
}

void Game::render() {
	//Clear screen
	SDL_Renderer* gRenderer = Engine::getInstance()->pass_renderer();
	int viewPortWidth = Engine::getInstance()->SCREEN_WIDTH;
	int viewPortHeight = Engine::getInstance()->SCREEN_HEIGHT;
	SDL_SetRenderDrawColor(gRenderer, 0x33, 0x33, 0x33, 0xFF);
	SDL_RenderClear(gRenderer);

	TextureManager* texMan = TextureManager::getInstance();
	//painter's algorithm
	//iterate through render lookup table and draw
	double angle;
	int x, y, w, h;
	for (Actor* actor : mActors) {
		auto components = actor->getComponents();
		for (Component* component : components) {
			if (component->get_cType() == ComponentType::SpriteComponent) {
				SpriteComponent* spriteComponent = dynamic_cast<SpriteComponent*>(component);
				int drawOrder = spriteComponent->getDrawOrder();
				mRenderLookupTable.insert({ drawOrder, spriteComponent });
				spriteComponent = nullptr;
			}
			else if (component->get_cType() == ComponentType::AnimSpriteComponent) {
				AnimSpriteComponent* aniSprite = dynamic_cast<AnimSpriteComponent*>(component);
				int drawOrder = aniSprite->getDrawOrder();
				mRenderLookupTable.insert({ drawOrder, aniSprite });
				aniSprite = nullptr;
			}
		}
	}
	for (auto element : mRenderLookupTable) {
		angle = Math::ToDegrees(element.second->passOwner()->getRot());
		//render static image
		element.second->Draw(texMan, angle);
	}

	texMan->render(gRenderer, "Score: ", (3 * viewPortWidth) / 4, 50);
	texMan->render(gRenderer, std::to_string(mScore), ((3 * viewPortWidth) / 4)+120, 52);

	mRenderLookupTable.clear();

	//Update screen
	SDL_RenderPresent(gRenderer);
	gRenderer = nullptr;
}

void Game::runFrame(float deltaTime) {
	processInput();
	update(deltaTime);
	//collision subroutine
	render();
}

void Game::runGame() {
	//run the first frame with a dummy timestep so can calculate deltaTime
	RSOS_Perf_Timer frameTimer;
	float frameCap = (1.f/FRAME_CAP);
	frameTimer.start();
	float deltaTime = frameCap;

	while (!quit) {
		Uint64 frameTicks = frameTimer.getTicks();
		deltaTime = static_cast<float>(frameTicks) / static_cast<float>(SDL_GetPerformanceFrequency());
		if (deltaTime >= frameCap) {
			runFrame(deltaTime);
			frameTimer.reset();
		}
		if (deltaTime < frameCap) {
			SDL_Delay(static_cast<Uint32>((frameCap - deltaTime) * 1000.0f)); // Convert to milliseconds
		}
	}
}
