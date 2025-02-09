#include"../include/Actor.h"
#include"../include/Game.h"
#include"../include/Random.h"
#include"../include/AudioManager.h"
#include<vector>

Actor::Actor(class Game* game,Vector2 pos, float scale, float rot) {
	set_state(EActive);
	mComponents = std::vector<class Component*>();
	set_sca(scale);
	set_rot(rot);
	set_pos(pos);
	mRecomputeWorldTransform = true;
}

Actor::~Actor() {
	for (Component* component : mComponents) {
		delete component;
	}
	mComponents.clear();
}

void Actor::update(float deltaTime) {
	ComputeWorldTransform();
	updateActor(deltaTime);
	mRecomputeWorldTransform = true;
	ComputeWorldTransform();
}

void Actor::updateActor(float deltaTime) {

}

void Actor::processInput(const uint8_t* keyState) {
	if (eState == EActive) {
		for (auto comp : mComponents) {
			comp->processInput(keyState);
		}
		actorInput(keyState);
	}
}

void Actor::actorInput(const uint8_t* keyState)
{
}

void Actor::ComputeWorldTransform()
{
	if (mRecomputeWorldTransform)
	{
		mRecomputeWorldTransform = false;
		// Scale, then rotate, then translate
		mWorldTransform = Matrix4::CreateScale(mScale);
		mWorldTransform *= Matrix4::CreateRotationZ(-mRotation);
		mWorldTransform *= Matrix4::CreateTranslation(Vector3(mPos.x, mPos.y, 0.0f));

		// Inform components world transform updated
		for (auto comp : mComponents)
		{
			comp->OnUpdateWorldTransform();
		}
	}
}

Ship::Ship(class Game* game, Vector2 pos, float scale, float rot) :Actor(game, pos, scale, rot) {
	mLaserCooldown = 0.0f;
	// Create a sprite component
	//SpriteComponent* sc = new SpriteComponent(this, 150);
	//sc->SetTexture(game->GetTexture("Assets/Ship.png"));
	AnimSpriteComponent* ac = new AnimSpriteComponent(this, 0, 15, 20, 24, true, { 2,5 }, 150,25);
	ac->addSequence(11);
	ac->set_loop(true);
	ac->setAngleOffset(90.0f);

	// Create an input component and set keys/speed
	InputComponent* ic = new InputComponent(this);
	ic->SetForwardKey(SDL_SCANCODE_W);
	ic->SetBackKey(SDL_SCANCODE_S);
	ic->SetClockwiseKey(SDL_SCANCODE_A);
	ic->SetCounterClockwiseKey(SDL_SCANCODE_D);
	ic->SetMaxForwardSpeed(150.0f);
	ic->SetMaxAngularSpeed(Math::Pi);
	ic->SetForwardAccel(20.0f);
	ic->SetAngularAccel(1.0f);
	ic->SetForwardDecel(2.5f);
	ic->SetAngularDecel(0.5f);


	CircleComponent* mCircle = new CircleComponent(this);
	mCircle->SetRadius(6.0f);

	addComponent(mCircle);
	addComponent(ac);
	addComponent(ic);
	set_aType(ActorType::Ship);
}

void Ship::updateActor(float deltaTime) {
	mLaserCooldown -= deltaTime;
}

void Ship::actorInput(const uint8_t* keyState)
{
	if (keyState[SDL_SCANCODE_SPACE] && mLaserCooldown <= 0.0f)
	{
		// Create a laser and set its position/rotation to mine
		AudioManager::getInstance()->playSound(0);
		Laser* laser = new Laser(Game::getInstance());
		laser->set_pos(getPos());
		laser->set_rot(getRot());
		Game::getInstance()->add_actor(laser);

		// Reset laser cooldown (half second)
		mLaserCooldown = 0.5f;
	}
}

CircleComponent* Ship::GetCircle() {
	CircleComponent* circleComp;
	for (class Component* component : getComponents()) {
		if (component->get_cType() == ComponentType::CircleComponent) {
			circleComp = dynamic_cast <CircleComponent*>(component);
			return circleComp;
		}
	}
}

Asteroid::Asteroid(class Game* game) :Actor(game) {
	Vector2 randPos = Random::GetVector(Vector2::Zero, Vector2(1024.0f, 768.0f));
	set_pos(randPos);
	set_rot(Random::GetFloatRange(0.0f, Math::TwoPi));
	set_sca(Random::GetFloatRange(0.5f, 3.0f));
	SpriteComponent* sc = new SpriteComponent(this, 2);
	sc->SetTexture(TextureManager::getInstance(),2);
	addComponent(sc);
	MoveComponent* mc = new MoveComponent(this);
	addComponent(mc);
	mc->setForwardSpeed(20.0f);
	CircleComponent* mCircle = new CircleComponent(this);
	mCircle->SetRadius(14.0f);
	addComponent(mCircle);
	set_aType(ActorType::Asteroid);
}

Asteroid::~Asteroid() {
}

void Asteroid::updateActor(float deltaTime) {
	// Do we intersect with an asteroid?
	std::vector<class Actor*> actors = Game::getInstance()->getActors();
	for (auto sh : actors)
	{
		if (sh->get_state() != Actor::EDead && sh->get_aType() == ActorType::Ship) {
			Ship* ship = dynamic_cast<Ship*>(sh);
			if (ship && GetCircle() && ship->GetCircle() && Intersect(*GetCircle(), *(ship->GetCircle())))
			{
				set_state(EDead);
				ship->set_state(EDead);
				AudioManager::getInstance()->playSound(1);
				Game* game = Game::getInstance();
				if (getSca() >= 1.0f) {
					Asteroid* asteroid_one = new Asteroid(game);
					Asteroid* asteroid_two = new Asteroid(game);
					asteroid_one->set_pos(Vector2(getPos().x - 20.f, getPos().y));
					asteroid_two->set_pos(Vector2(getPos().x + 20.f, getPos().y));
					float maxScale = getSca();
					if (asteroid_one->getSca() > maxScale - 0.5f) {
						asteroid_one->set_sca(maxScale - 0.5f);
					}
					if (asteroid_two->getSca() > maxScale - 0.5f) {
						asteroid_two->set_sca(maxScale - 0.5f);
					}
					game->add_actor(asteroid_one);
					game->add_actor(asteroid_two);
				}
				game->start_gameOverTimer();
			}
		}
	}
}

CircleComponent* Asteroid::GetCircle() {
	CircleComponent* circleComp;
	for (class Component* component : getComponents()) {
		if (component->get_cType() == ComponentType::CircleComponent) {
			circleComp = dynamic_cast <CircleComponent*>(component);
			return circleComp;
		}
	}
}

Laser::Laser(Game* game):Actor(game)
{
	mDeathTimer = 1.0f;
	set_sca(0.2f);
	// Create a sprite component
	SpriteComponent* sc = new SpriteComponent(this,4);

	// Create a move component, and set a forward speed
	MoveComponent* mc = new MoveComponent(this);
	mc->setForwardSpeed(200.0f);

	// Create a circle component (for collision)
	CircleComponent* mCircle = new CircleComponent(this);
	mCircle->SetRadius(1.0f);

	addComponent(sc);
	addComponent(mc);
	addComponent(mCircle);
	set_aType(ActorType::Laser);
}

Laser::~Laser() {
}

void Laser::updateActor(float deltaTime)
{
	noRender = false;
	// If we run out of time, laser is dead
	mDeathTimer -= deltaTime;
	if (mDeathTimer <= 0.0f)
	{
		set_state(EDead);
	}
	else
	{
		// Do we intersect with an asteroid?
		std::vector<class Actor*> actors = Game::getInstance()->getActors();
		for (auto ast : actors)
		{
			if (ast->get_state()!=Actor::EDead && ast->get_aType() == ActorType::Asteroid) {
				Asteroid* aster = dynamic_cast<Asteroid*>(ast);
				if (aster && GetCircle() && aster->GetCircle() && Intersect(*GetCircle(), *(aster->GetCircle())))
				{
					//The first asteroid we intersect with,
					// set ourselves and the asteroid to dead
					set_state(EDead);
					aster->set_state(EDead);
					AudioManager::getInstance()->playSound(1);
					Game* game = Game::getInstance();
					game->add_score(100);
					if (aster->getSca() >= 1.0f) {
						Asteroid* asteroid_one = new Asteroid(game);
						Asteroid* asteroid_two = new Asteroid(game);
						asteroid_one->set_pos(Vector2(aster->getPos().x - 20.f, aster->getPos().y));
						asteroid_two->set_pos(Vector2(aster->getPos().x + 20.f, aster->getPos().y));
						float maxScale = aster->getSca();
						if (asteroid_one->getSca() > maxScale - 0.5f) {
							asteroid_one->set_sca(maxScale - 0.5f);
						}
						if (asteroid_two->getSca() > maxScale - 0.5f) {
							asteroid_two->set_sca(maxScale - 0.5f);
						}
						game->add_actor(asteroid_one);
						game->add_actor(asteroid_two);
					}
					break;
				}
			}
		}
	}
}

CircleComponent* Laser::GetCircle() {
	CircleComponent* circleComp;
	for (class Component* component : getComponents()) {
		if (component->get_cType() == ComponentType::CircleComponent) {
			circleComp = dynamic_cast <CircleComponent*>(component);
			return circleComp;
		}
	}
}

WarpZone::WarpZone(Game* game) : Actor(game) {
	Engine* engine = Engine::getInstance();
	WarpComponent* wc = new WarpComponent(this, 10,-engine->SCREEN_WIDTH/2,-engine->SCREEN_HEIGHT/2,engine->SCREEN_WIDTH/2,engine->SCREEN_HEIGHT/2);
	SpriteComponent* sc = new SpriteComponent(this, 1, 1,1);
	addComponent(wc);
	addComponent(sc);
	set_aType(ActorType::WarpZone);
}