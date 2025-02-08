#include"../include/Component.h"
#include"../include/TextureManager.h"
#include"../include/Engine.h"
#include"../include/Utilities.h"
#include"Math.h"
#include<cmath>

Component::Component(class Actor* owner, int uO){
	mOwner = owner;
	mUpdateOrder = uO;
	cType = ComponentType::Component;
}

Component::~Component() {
	mOwner = nullptr;
}

void Component::update(float deltaTime) {

}

void SpriteComponent::Draw(TextureManager* textureManager) {
	SDL_Log("Sprite Component draw");
	Shader* shader = textureManager->getShader(0);
	Texture* texture = textureManager->getTexture(texIndex);
	if (shader && texture) {
		// Scale the quad by the width/height of texture
		Matrix4 scaleMat = Matrix4::CreateScale(
			static_cast<float>(mTexWidth),
			static_cast<float>(mTexHeight),
			1.0f);

		Matrix4 world = scaleMat * mOwner->GetWorldTransform();

		// Since all sprites use the same shader/vertices,
		// the game first sets them active before any sprite draws

		// Set world transform
		shader->SetMatrixUniform("uWorldTransform", world);
		// Set current texture
		texture->SetActive();
		// Draw quad
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	}

	/*
	TextureMetadata mTextureMetadata = textureManager->fetchData(texIndex);
	double adjustedAngle = angleOffset - angle;
	//textureManager->render(Engine::getInstance()->pass_renderer(), texIndex, mOwner->getPos().x, mOwner->getPos().y, mTextureMetadata.alpha, mTextureMetadata.colorMod, mTextureMetadata.blendMode,mOwner->getSca(),adjustedAngle);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	*/

}

void SpriteComponent::SetTexture(TextureManager* textureManager, int index) {
	if (textureManager->fetchTextureListLength() > index && index >= 0) {
		texIndex = index;
		Texture* mTex = textureManager->getTexture(index);
		if (mTex) {
			mTexWidth = mTex->GetWidth();
			mTexHeight = mTex->GetHeight();
		}
	}
}

void SpriteComponent::setDrawOrder(TextureManager* textureManager, int drawOrder) {
	mDrawOrder = drawOrder;
}

SpriteComponent::SpriteComponent(class Actor* owner, int texIndex, int drawOrder, int uO) : Component(owner, uO) {
	mDrawOrder = drawOrder;
	mOwner = owner;
	mTexWidth = 0;
	mTexHeight = 0;
	SetTexture(TextureManager::getInstance(), texIndex);
	cType = ComponentType::SpriteComponent;
}

AnimSpriteComponent::AnimSpriteComponent(Actor* owner, int texIndex, int cWidth, int cHeight, int fpsA, bool loop, Point offset, float aOffset, int drawOrder, int uO):SpriteComponent(owner,texIndex,drawOrder,uO)
{
	cellWidth = cWidth;
	cellHeight = cHeight;
	mLoop = loop;
	sequences = std::map<int, int>();
	currentCell = { 0,0 };
	fps = fpsA;
	animationTimer = RSOS_Timer();
	mAnimating = false;
	mOffset = offset;
	angleOffset = aOffset;
	cType = ComponentType::AnimSpriteComponent;
}

void AnimSpriteComponent::update(float deltaTime) {
	if (mAnimating) {
		float timerTick = animationTimer.getTicks();
		float deltaAnimation = (timerTick / 1000.0f) - (1.f / fps);
		if (deltaAnimation > 1.0f) {
			if (mLoop) {
				deltaAnimation = fmod(deltaAnimation,1.0f);
			}
			else {
				deltaAnimation = 1.0f;
			}
		}
		else if (deltaAnimation < 0.0f) {
			deltaAnimation = 0.0f;
		}
		int currentFrame = static_cast<int>(deltaAnimation * sequences[currentCell.y]);
		currentCell.x = currentFrame;
	}
}

void AnimSpriteComponent::animate() {
	if (!mAnimating) {
		if (!animationTimer.isStarted()) {
			animationTimer.start();
		}
		if (animationTimer.isPaused()) {
			animationTimer.unpause();
		}
		mAnimating = true;
	}
	else {
		animationTimer.pause();
		mAnimating = false;
	}
}

void AnimSpriteComponent::resetAnimation() {
	animationTimer.stop();
	mAnimating = false;
	currentCell.x = 0;
}

void AnimSpriteComponent::Draw(TextureManager* textureManager) {
	SDL_Log("Anim Sprite Component draw");
	Shader* shader = textureManager->getShader(0);
	Texture* texture = textureManager->getTexture(texIndex);
	if (shader && texture) {
		// Scale the quad by the width/height of texture
		Matrix4 scaleMat = Matrix4::CreateScale(
			static_cast<float>(mTexWidth),
			static_cast<float>(mTexHeight),
			1.0f);

		Matrix4 world = scaleMat * mOwner->GetWorldTransform();

		// Since all sprites use the same shader/vertices,
		// the game first sets them active before any sprite draws

		// Set world transform
		shader->SetMatrixUniform("uWorldTransform", world);
		// Set current texture
		texture->SetActive();
		// Draw quad
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	}

	/*
	TextureMetadata mTextureMetadata = textureManager->fetchData(texIndex);
	Point adjustedStart = { mOffset.x + (currentCell.x * cellWidth),mOffset.y + (currentCell.y * cellHeight) };
	SDL_Rect clip;
	clip = { adjustedStart.x,adjustedStart.y,cellWidth,cellHeight };
	SDL_Rect* clipP = &clip;
	double adjustedAngle = angleOffset-angle;
	textureManager->render(Engine::getInstance()->pass_renderer(), texIndex, mOwner->getPos().x, mOwner->getPos().y, mTextureMetadata.alpha, mTextureMetadata.colorMod, mTextureMetadata.blendMode, mOwner->getSca(), adjustedAngle, clipP);
	clipP = nullptr;
	*/
}

void AnimSpriteComponent::addSequence(int length) {
	int highestKey = 0; 
	int newKey = highestKey;
	if (!sequences.empty()) {
		highestKey = sequences.rbegin()->first;
		newKey = highestKey + 1;
	}
	sequences[newKey] = length;
}

void AnimSpriteComponent::removeSequence(int sequence) {
	//if the sequence is in the map, remove it
	if (sequences.find(sequence)!=sequences.end()) {
		sequences.erase(sequence);
	}
}

void AnimSpriteComponent::resizeSequence(int sequence, int length) {
	if (sequences.find(sequence) != sequences.end()) {
		sequences[sequence]=length;
	}
}

void AnimSpriteComponent::changeActiveSequence(int sequence){
	if (sequences.find(sequence) != sequences.end()) {
		currentCell.y = sequence;
		currentCell.x = 0;
		resetAnimation();
		animate();
	}
}

BGSpriteComponent::BGSpriteComponent(Actor* owner, int texIndex, float clipW, float clipH, float scrollSpeedX, float scrollSpeedY,int drawOrder, int uO) : SpriteComponent(owner,texIndex,drawOrder,uO){
	mOffset = Vector2::Zero;
	mClipSize = Vector2(clipW,clipH);
	mScrollSpeed.x = scrollSpeedX;
	mScrollSpeed.y = scrollSpeedY;
	mScrolling.xFlag = false;
	mScrolling.xFlag = false;
	mLooping = { false,false };
	cType = ComponentType::BGSpriteComponent;
}

void BGSpriteComponent::update(float deltaTime) {
	/*
	float width = getWidth();
	float height = getHeight();
	if (mScrolling.xFlag) {
		if (mOffset.x + mClipSize.x <= width || mScrollSpeed.x>0.0f) {
			mOffset.x += (mScrollSpeed.x * deltaTime);
		}
		else if (mOffset.x >= 0.0f || mScrollSpeed.x<0.0f) {
			mOffset.x += (mScrollSpeed.x * deltaTime);
		}
		if (mOffset.x + mClipSize.x > width) {
			if (mLooping.xFlag) {
				mOffset.x = 0;
			}
			else {
				mOffset.x = width - mClipSize.x;
			}
		}
		else if (mOffset.x<0){
			if (mLooping.xFlag) {
				mOffset.x = width - mClipSize.x; 
			}
			else {
				mOffset.x = 0;
			}
		}
	}
	if (mScrolling.yFlag) {
		if (mOffset.y + mClipSize.y <= height || mScrollSpeed.y > 0.0f) {
			mOffset.y += (mScrollSpeed.y * deltaTime);
		}
		else if (mOffset.y >= 0.0f || mScrollSpeed.y < 0.0f) {
			mOffset.y += (mScrollSpeed.y * deltaTime);
		}
		if (mOffset.y + mClipSize.y > height) {
			if (mLooping.yFlag) {
				mOffset.y = 0;
			}
			else {
				mOffset.y = height - mClipSize.y;
			}
		}
		else if (mOffset.y < 0) {
			if (mLooping.yFlag) {
				mOffset.y = width - mClipSize.y;
			}
			else {
				mOffset.y = 0;
			}
		}
	}*/
}

void BGSpriteComponent::Draw(TextureManager* textureManager) {

	/*
	TextureMetadata mTextureMetadata = textureManager->fetchData(texIndex);
	SDL_Rect clip;
	clip = { (int)mOffset.x,(int)mOffset.y,(int)mClipSize.x,(int)mClipSize.y };
	SDL_Rect* clipP = &clip;
	textureManager->render(Engine::getInstance()->pass_renderer(), texIndex, mOffset.x, mOffset.y, mTextureMetadata.alpha, mTextureMetadata.colorMod, mTextureMetadata.blendMode, mOwner->getSca(), angle, clipP);
	clipP = nullptr;
	*/
}

MoveComponent::MoveComponent(class Actor* owner, int uO) : Component(owner, uO) {
	mAngularSpeed = 0.0f;
	mForwardSpeed = 0.0f;
}

void MoveComponent::update(float deltaTime) {
	if (!Math::NearZero(mAngularSpeed))
	{
		float rot = passOwner()->getRot();
		rot += mAngularSpeed * deltaTime;
		mOwner->set_rot(rot);
	}
	if (!Math::NearZero(mForwardSpeed))
	{
		Vector2 pos = passOwner()->getPos();
		pos.x += (mOwner->getForward().x)*mForwardSpeed * deltaTime;
		pos.y += (mOwner->getForward().y)*mForwardSpeed * deltaTime;
		mOwner->set_pos(pos);
	}
}

InputComponent::InputComponent(class Actor* owner) :MoveComponent(owner) {
	SetForwardKey(0);
	SetBackKey(0);
	SetClockwiseKey(0);
	SetCounterClockwiseKey(0);
	cType = ComponentType::InputComponent;
}

void InputComponent::processInput(const uint8_t* keyState)
{
	// Calculate forward speed for MoveComponent
	AnimSpriteComponent* animSprite=nullptr;
	MoveComponent* moveComponent=nullptr;
	for (auto component : mOwner->getComponents()) {
		if (component->get_cType() == ComponentType::AnimSpriteComponent) {
			animSprite = dynamic_cast<AnimSpriteComponent*>(component);
		}
	}
	float forwardSpeed = getForwardSpeed();
	// Calculate angular speed for MoveComponent
	float angularSpeed = getAngularSpeed();

	if (keyState[mForwardKey])
	{
		if (animSprite != nullptr) {
		  if(!animSprite->isAnimating())
			{
				animSprite->animate();
			}
		}
		if(forwardSpeed+mForwardAccel<=mMaxForwardSpeed)
			forwardSpeed += mForwardAccel;
	}
	else if (animSprite != nullptr && animSprite->isAnimating()) {
		animSprite->resetAnimation();
	}
	if (keyState[mBackKey])
	{

		if (forwardSpeed - (mForwardAccel / 2.0) >= -(mMaxForwardSpeed) / 2.0)
			forwardSpeed -= (mForwardAccel/2.0);
	}

	if (keyState[mClockwiseKey])
	{
		if (angularSpeed + mAngularAccel <= mMaxAngularSpeed)
			angularSpeed += mMaxAngularSpeed;
	}
	if (keyState[mCounterClockwiseKey])
	{
		if (angularSpeed - mAngularAccel >= -mMaxAngularSpeed)
		angularSpeed -= mMaxAngularSpeed;
	}

	if (!keyState[mForwardKey] && !keyState[mBackKey]) {
		if (forwardSpeed > 0.0) {
			forwardSpeed -= mForwardDecelFactor;
			if (forwardSpeed < 0.0)
				forwardSpeed = 0;
		}
		else if (forwardSpeed < 0.0) {
			forwardSpeed += mForwardDecelFactor;
			if (forwardSpeed > 0.0)
				forwardSpeed = 0.0;
		}
	}
	if (!keyState[mClockwiseKey] && !keyState[mCounterClockwiseKey]) {
			if (angularSpeed > 0.0) {
				angularSpeed -= mAngularDecelFactor;
				if (angularSpeed < 0.0)
					angularSpeed = 0;
			}
			else if (angularSpeed < 0.0) {
				angularSpeed += mAngularDecelFactor;
				if (angularSpeed > 0.0)
					angularSpeed = 0.0;
			}
	}

	setForwardSpeed(forwardSpeed);
	setAngularSpeed(angularSpeed);
	animSprite = nullptr;
}

CircleComponent::CircleComponent(class Actor* owner)
	:Component(owner)
{
	SetRadius(0.0f);
	cType = ComponentType::CircleComponent;
}

const Vector2& CircleComponent::GetCenter() const
{
	Vector2 center = Vector2(mOwner->getPos().x + GetRadius(), mOwner->getPos().y + GetRadius());
	return center;
}

float CircleComponent::GetRadius() const
{
	return mOwner->getSca() * mRadius;
}

bool Intersect(const CircleComponent& a, const CircleComponent& b)
{
	Vector2 aCenter = a.GetCenter();
	Vector2 bCenter = b.GetCenter();
	//SDL_Log("a.Center: x:%f, y:%f", aCenter.x, aCenter.y);
	//SDL_Log("b.Center: x:%f, y:%f", bCenter.x, bCenter.y);

	// Calculate distance squared
	Vector2 diff = aCenter - bCenter;
	//SDL_Log("Diff x:%f, y:%f", diff.x, diff.y);
	float distSq = diff.LengthSq();
	//SDL_Log("distSq: %f", distSq);

	// Calculate sum of radii squared
	float radiiSq = a.GetRadius() + b.GetRadius();
	radiiSq *= radiiSq;

	return distSq <= radiiSq;
}

WarpComponent::WarpComponent(Actor* owner,int uO,int xOff, int yOff, int wArg, int hArg) : Component(owner,uO) {
	window.x = xOff;
	window.y = yOff;
	window.w = wArg;
	window.h = hArg;
	cType = ComponentType::WarpComponent;
}

void WarpComponent::update(float deltaTime) {
	Game* game = Game::getInstance();
	for (Actor* actor : game->getActors()) {
		if (actor->get_aType() != ActorType::WarpZone) {
			Vector2 aPos = actor->getPos();
			if (aPos.x > window.w + 50) {
				actor->set_pos(Vector2(0, aPos.y));
			}
			else if (aPos.x < window.x-50) {
				actor->set_pos(Vector2(window.w-20, aPos.y));
			}
			if (aPos.y > window.h+50) {
				actor->set_pos(Vector2(aPos.x, 0));
			}
			else if (aPos.y < window.y-50) {
				actor->set_pos(Vector2(aPos.x, window.h-20));
			}
		}
	}
}