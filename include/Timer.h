#pragma once
#ifndef RSOS_TIMER
#define RSOS_TIMER
#include<SDL.h>
//The application time based timer
class RSOS_Timer
{
public:
    //Initializes variables
    RSOS_Timer();

    //The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();
    void reset();

    //Gets the timer's time
    Uint32 getTicks();

    //Checks the status of the timer
    bool isStarted();
    bool isPaused();

protected:
    //The clock time when the timer started
    Uint32 mStartTicks;

    //The ticks stored when the timer was paused
    Uint32 mPausedTicks;

    //The timer status
    bool mPaused;
    bool mStarted;
};

class RSOS_Perf_Timer : protected RSOS_Timer {
public:
    RSOS_Perf_Timer();
    void start();
    void stop();
    void pause();
    void unpause();
    void reset();
    //Gets the timer's time
    Uint32 getTicks();


};
#endif