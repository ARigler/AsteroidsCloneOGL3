#include"../include/Timer.h"

//Initializes variables
RSOS_Timer::RSOS_Timer() {
	mStartTicks = 0;
	mPausedTicks = 0;

	mPaused = false;
	mStarted = false;
}

//The various clock actions
void RSOS_Timer::start() {
	mStarted = true;
	mPaused = false;

	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}
void RSOS_Timer::stop() {
	mStarted = false;
	mPaused = false;

	mStartTicks = 0;
	mPausedTicks = 0;
}

void RSOS_Timer::pause() {
	if (mStarted && !mPaused) {
		mPaused = true;

		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}
	
void RSOS_Timer::unpause() {
	if (mStarted && mPaused) {
		mPaused = false;

		mStartTicks = SDL_GetTicks() - mPausedTicks;

		mPausedTicks = 0;
	}
}

void RSOS_Timer::reset() {
	mStarted = true;
	mPaused = false;

	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

//Gets the timer's time
Uint32 RSOS_Timer::getTicks() {
	Uint32 time = 0;

	if (mStarted) {
		if (mPaused) {
			time = mPausedTicks;
		}
		else {
			time = SDL_GetTicks() - mStartTicks;
		}
	}

	return time;
}

//Checks the status of the timer
bool RSOS_Timer::isStarted() {
	return mStarted;
}
bool RSOS_Timer::isPaused() {
	return mPaused && mStarted;
}

RSOS_Perf_Timer::RSOS_Perf_Timer() {
	RSOS_Timer();
}

//The various clock actions
void RSOS_Perf_Timer::start() {
	mStarted = true;
	mPaused = false;

	mStartTicks = SDL_GetPerformanceCounter();
	mPausedTicks = 0;
}
void RSOS_Perf_Timer::stop() {
	mStarted = false;
	mPaused = false;

	mStartTicks = 0;
	mPausedTicks = 0;
}

void RSOS_Perf_Timer::pause() {
	if (mStarted && !mPaused) {
		mPaused = true;

		mPausedTicks = SDL_GetPerformanceCounter() - mStartTicks;
		mStartTicks = 0;
	}
}

void RSOS_Perf_Timer::unpause() {
	if (mStarted && mPaused) {
		mPaused = false;

		mStartTicks = SDL_GetPerformanceCounter() - mPausedTicks;

		mPausedTicks = 0;
	}
}

void RSOS_Perf_Timer::reset() {
	mStarted = true;
	mPaused = false;

	mStartTicks = SDL_GetPerformanceCounter();
	mPausedTicks = 0;
}

//Gets the timer's time
Uint32 RSOS_Perf_Timer::getTicks() {
	Uint32 time = 0;

	if (mStarted) {
		if (mPaused) {
			time = mPausedTicks;
		}
		else {
			time = SDL_GetPerformanceCounter() - mStartTicks;
		}
	}

	return time;
}