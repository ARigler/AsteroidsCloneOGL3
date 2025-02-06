#pragma once
#ifndef RSOS_COMPONENT
#define RSOS_COMPONENT
#include"Structures.h"
#include"TextureManager.h"
#include"Timer.h"
#include"Math.h"
#include"Engine.h"
#include<vector>
#include<SDL.h>
#include<map>

enum class ComponentType {
	Component,
	SpriteComponent,
	AnimSpriteComponent,
	BGSpriteComponent,
	MoveComponent,
	InputComponent,
	CircleComponent,
	WarpComponent
};

class Component {
public:
	Component(class Actor* owner, int uO = 100);
	virtual ~Component();
	virtual void update(float deltaTime);
	virtual void processInput(const uint8_t* keyState) {}
	class Actor* passOwner() const { return mOwner; }
	int get_uO() const { return mUpdateOrder; }
	ComponentType get_cType() const { return cType; }
protected:
	class Actor* mOwner;
	int mUpdateOrder;
	ComponentType cType;
};

class SpriteComponent : public Component {
public:
	SpriteComponent(class Actor* owner, int texIndex,int drawOrder = 100, int uO=100);
	virtual void Draw(TextureManager* textureManager,double angle,SDL_RendererFlip renderFlip = SDL_FLIP_NONE);
	virtual void SetTexture(TextureManager* textureManager,int index);
	int getTexIndex() const { return texIndex; }
	int getDrawOrder() const { return mDrawOrder; }
	void setDrawOrder(TextureManager* textureManager,int drawOrder);
	int getWidth() const { return TextureManager::getInstance()->fetchData(texIndex).width; }
	int getHeight() const { return TextureManager::getInstance()->fetchData(texIndex).height; }
	float getAngleOffset()const { return angleOffset; }
	void setAngleOffset(int thet) { angleOffset = thet; }
protected:
	int texIndex;
	int mDrawOrder;
	float angleOffset;
};

class AnimSpriteComponent : public SpriteComponent {
public:
	AnimSpriteComponent(class Actor* owner, int texIndex, int cWidth, int cHeight, int fps, bool loop = false, Point offset = {0,0},float aOffset=0.0f, int drawOrder = 100, int uO = 100);
	void update(float deltaTime) override;
	void Draw(TextureManager* textureManager, double angle, SDL_RendererFlip renderFlip = SDL_FLIP_NONE) override;
	void setTexIndex(TextureManager* texMan,int index) { texIndex=index; }
	float getAnimFPS()const { return fps; }
	Point getOffset()const { return mOffset; }
	void animate();
	void resetAnimation();
	void setAnimFPS(int afps) { fps = afps; }
	void setOffset(int x, int y) { mOffset.x = x; mOffset.y = y; }
	Point getCellDimensions()const { return { cellWidth,cellHeight }; }
	void setCellDimensions(int w, int h) { cellWidth = w; cellHeight = h; }
	void addSequence(int length);
	void removeSequence(int sequence);
	void resizeSequence(int sequence, int length);
	void changeActiveSequence(int sequence);
	void set_loop(bool loopArg) { mLoop = loopArg; }
	bool isAnimating()const { return mAnimating; }
private:
	int fps;
	int cellWidth;
	int cellHeight;
	std::map<int, int> sequences;
	bool mLoop;
	bool mAnimating;
	RSOS_Timer animationTimer;
	Point currentCell;
	Point mOffset;
};

class BGSpriteComponent : public SpriteComponent {
public:
	BGSpriteComponent(class Actor* owner, int texIndex, float clipW, float clipH, float scrollSpeedX, float scrollSpeedY, int drawOrder = 100, int uO = 100);
	void update(float deltaTime) override;
	void Draw(TextureManager* textureManager, double angle, SDL_RendererFlip renderFlip = SDL_FLIP_NONE) override;
	void setTexIndex(TextureManager* texMan, int index) {
		if (texMan->fetchTextureListLength() > index && index >= 0) {
			texIndex = index;
		}
	}
	void setClipSize(const Vector2& size) { mClipSize = size; }
	void setScrollSpeed(float speedX, float speedY) { mScrollSpeed.x = speedX; mScrollSpeed.y = speedY; }
	void scroll(bool x, bool y) { mScrolling.xFlag = x; mScrolling.yFlag = y; }
	int getTexIndex()const { return texIndex; }
	Vector2 getScrollSpeed() const { return mScrollSpeed; }
private:
	Vector2 mOffset;
	Vector2 mClipSize;
	Vector2 mScrollSpeed;
	DualFlag mScrolling;
	DualFlag mLooping;
};

class MoveComponent : public Component {
public:
	MoveComponent(class Actor* owner, int updateOrder = 10);
	void update(float deltaTime) override;
	float getAngularSpeed()const { return mAngularSpeed; }
	float getForwardSpeed()const { return mForwardSpeed; }
	void setAngularSpeed(float speed) { mAngularSpeed = speed; }
	void setForwardSpeed(float speed) { mForwardSpeed = speed; }
private:
	float mAngularSpeed;
	float mForwardSpeed;
};

class InputComponent : public MoveComponent {
public:
	InputComponent(class Actor* owner);
	void processInput(const uint8_t* keyState) override;

	float GetMaxForward() const { return mMaxForwardSpeed; }
	float GetMaxAngular() const { return mMaxAngularSpeed; }
	float GetForwardAccel() const { return mForwardAccel; }
	float GetAngularAccel() const { return mAngularAccel; }
	float GetForwardDecel() const { return mForwardDecelFactor; }
	float GetAngularDecel() const { return mAngularDecelFactor; }
	int GetForwardKey() const { return mForwardKey; }
	int GetBackKey() const { return mBackKey; }
	int GetClockwiseKey() const { return mClockwiseKey; }
	int GetCounterClockwiseKey() const { return mCounterClockwiseKey; }

	void SetMaxForwardSpeed(float speed) { mMaxForwardSpeed = speed; }
	void SetMaxAngularSpeed(float speed) { mMaxAngularSpeed = speed; }
	void SetForwardAccel(float accel) { mForwardAccel = accel; }
	void SetAngularAccel(float accel) { mAngularAccel = accel; }
	void SetForwardDecel(float decel) { mForwardDecelFactor = decel; }
	void SetAngularDecel(float decel) { mAngularDecelFactor = decel; }
	void SetForwardKey(int key) { mForwardKey = key; }
	void SetBackKey(int key) { mBackKey = key; }
	void SetClockwiseKey(int key) { mClockwiseKey = key; }
	void SetCounterClockwiseKey(int key) { mCounterClockwiseKey = key; }
private:
	float mMaxForwardSpeed;
	float mMaxAngularSpeed;
	float mForwardAccel;
	float mAngularAccel;
	float mForwardDecelFactor;
	float mAngularDecelFactor;
	int mForwardKey;
	int mBackKey;
	int mClockwiseKey;
	int mCounterClockwiseKey;
};

class CircleComponent : public Component
{
public:
	CircleComponent(class Actor* owner);

	void SetRadius(float radius) { mRadius = radius; }
	float GetRadius() const;

	const Vector2& GetCenter() const;
private:
	float mRadius;
};

bool Intersect(const CircleComponent& a, const CircleComponent& b);

class WarpComponent :public Component {
public:
	WarpComponent(class Actor* owner, int uO=10, int xOff=0, int yOff=0, int wArg = 80, int hArg=80);
	void update(float deltaTime) override;
private:
	SDL_Rect window;
};

#endif