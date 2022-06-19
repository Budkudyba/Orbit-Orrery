//=======================================================================================
// GameTimer.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef GAMETIMER_H
#define GAMETIMER_H


class GameTimer
{
public:
	GameTimer();

	float getGameTime()const;  // time played
	float getDeltaTime()const; // time since tick
	float getPausedTime()const;// in seconds
	float getElapsedTime()const;// time since game start
	bool  getPauseState();//true if paused

	void reset(); // Call before message loop.
	void start(); // Call when unpaused.
	void stop();  // Call when paused.
	void tick();  // Call every frame.

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;//base from current
	__int64 mPausedTime;//all paused time
	__int64 mStopTime;//the time when last pause started
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
};

#endif // GAMETIMER_H