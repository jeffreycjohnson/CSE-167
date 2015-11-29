#include "Timer.h"
#include <iostream>
#include <glfw3.h>

using namespace std;

double Timer::startTime, Timer::oldTime, Timer::currentTime;
double Timer::mDeltaTime, Timer::mTime, Timer::mTimeScale, Timer::mUnscaledDeltaTime, Timer::mUnscaledTime, Timer::cycleTime;
int Timer::frames;
float Timer::interval;

void Timer::init(float interval)
{
	startTime = oldTime = currentTime = glfwGetTime();
	mDeltaTime = mTime = mUnscaledDeltaTime = mUnscaledTime = cycleTime = 0;
	mTimeScale = 1;
	frames = 0;
	Timer::interval = interval;
}

void Timer::update()
{
	oldTime = currentTime;
	currentTime = glfwGetTime();

	mDeltaTime = (float)(currentTime - oldTime) * mTimeScale;
	mTime += mDeltaTime;
	mUnscaledDeltaTime = (float)(currentTime - oldTime);
	mUnscaledTime += mUnscaledDeltaTime;

	cycleTime += mUnscaledDeltaTime;
	frames++;

	if (cycleTime > interval)
	{
		cout << cycleTime / frames * 1000 << " ms (" << frames / cycleTime << " fps)" << endl;
		cycleTime = 0;
		frames = 0;
	}
}

double Timer::deltaTime()
{
	return mDeltaTime;
}

double Timer::time()
{
	return mTime;
}

double Timer::unscaledDeltaTime()
{
	return mUnscaledDeltaTime;
}

double Timer::unscaledTime()
{
	return mUnscaledTime;
}

double Timer::timeScale()
{
	return mTimeScale;
}

void Timer::timeScale(double newScale)
{
	mTimeScale = newScale;
}