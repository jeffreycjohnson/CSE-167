#ifndef INCLUDE_TIME_H
#define INCLUDE_TIME_H
#include <time.h>

class Timer
{
private:
	static clock_t startTime, oldTime, currentTime;
	static float mDeltaTime, mTime, mTimeScale, mUnscaledDeltaTime, mUnscaledTime;

public:
	static void init();
	static void update();

	static float deltaTime();
	static float time();
	static float unscaledDeltaTime();
	static float unscaledTime();
	static float timeScale();
	static void timeScale(float newScale);
};

#endif