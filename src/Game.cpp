#include "../include/Engine.h"
#include"../include/Game.h"
#include"../include/Timer.h"
#include"../include/TextureManager.h"
#include"../include/AudioManager.h"
#include"../include/Component.h"
#include"../include/Random.h"
#include"../include/Math.h"
#include<iostream>
#include<string>
#include<algorithm>
#include<SDL.h>
#include<SDL_mixer.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<map>
#include<functional>
#include<gl/glew.h>
#include<GL/GL.h>


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
	//texMan->loadFromRenderedText(std::to_string(mScore), gFont, engine->pass_renderer());
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

	TextureManager* texMan = TextureManager::getInstance();
	AudioManager* audMan = AudioManager::getInstance();
	Shader* spriteShader = texMan->getShader(0);

	spriteShader->SetActive();
	texMan->loadTexture("assets/Ship1.png");
	texMan->loadTexture("assets/Space.png");
	texMan->loadTexture("assets/A1.png");
	texMan->loadTexture("assets/A2.png");
	texMan->loadTexture("assets/P7.png");

	//spriteShader->SetInt("Ship3", 1);
	//spriteShader->SetInt("Space", 1);
	//spriteShader->SetInt("A1", 1);
	//spriteShader->SetInt("A2", 1);
	//spriteShader->SetInt("P7", 1);

	audMan->loadClipFromFile("assets/151021__bubaproducer__laser-shot-big-3.wav");
	audMan->loadClipFromFile("assets/446624__idkmrgarcia__explosion2.wav");


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

	TextureManager* texMan = TextureManager::getInstance();


	Ship* ship = new Ship(this, Vector2(0, 0), 1.0f);
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
		Ship* ship = new Ship(this, Vector2(0, 0), 1.0f);
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
	TextureManager* texMan = TextureManager::getInstance();

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//SDL_SetRenderDrawColor(gRenderer, 0x33, 0x33, 0x33, 0xFF);
	//SDL_RenderClear(gRenderer);

	/*
	double angle;
	int x, y, w, h;
	//painter's algorithm
	//iterate through render lookup table and draw
	*/
	for (Actor* actor : mActors) {
		SDL_Log("actor %p", actor);
		auto components = actor->getComponents();
		for (Component* component : components) {
			SDL_Log("component %p", component);
			if (component->get_cType() == ComponentType::SpriteComponent) {
				if (actor->get_aType() == ActorType::Laser) {
					Laser* laser = dynamic_cast<Laser*>(actor);
					if (laser->getRenderState()) {
						continue;
					}
				}
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
	
	texMan->getShader(0)->SetActive();
	texMan->getVA(0)->setActive();


	for (auto element : mRenderLookupTable) {
		//angle = Math::ToDegrees(element.second->passOwner()->getRot());
		//render static image
		SDL_Log("DrawOrder %i, function %p", element.first,element.second);
		element.second->Draw(texMan);
	}

	mRenderLookupTable.clear();

	/*
	texMan->render(gRenderer, "Score: ", (3 * viewPortWidth) / 4, 50);
	texMan->render(gRenderer, std::to_string(mScore), ((3 * viewPortWidth) / 4)+120, 52);
	
	*/		

	//Update screen
	SDL_GL_SwapWindow(Engine::getInstance()->pass_window());
	//SDL_RenderPresent(gRenderer);
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
