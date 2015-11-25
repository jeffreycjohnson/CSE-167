#include "Timer.h"
#include <iostream>
#include <ctime>

clock_t Timer::startTime, Timer::oldTime, Timer::currentTime;
float Timer::mDeltaTime, Timer::mTime, Timer::mTimeScale, Timer::mUnscaledDeltaTime, Timer::mUnscaledTime;

void Timer::init()
{
	startTime = oldTime = currentTime = std::clock();
	mDeltaTime = mTime = mUnscaledDeltaTime = mUnscaledTime = 0;
	mTimeScale = 1;
}

void Timer::update()
{
	oldTime = currentTime;
	currentTime = std::clock();

	mDeltaTime = (float)(currentTime - oldTime) / CLOCKS_PER_SEC * mTimeScale;
	mTime += mDeltaTime;
	mUnscaledDeltaTime = (float)(currentTime - oldTime) / CLOCKS_PER_SEC;
	mUnscaledTime += mUnscaledDeltaTime;
}

float Timer::deltaTime()
{
	return mDeltaTime;
}

float Timer::time()
{
	return mTime;
}

float Timer::unscaledDeltaTime()
{
	return mUnscaledDeltaTime;
}

float Timer::unscaledTime()
{
	return mUnscaledTime;
}

float Timer::timeScale()
{
	return mTimeScale;
}

void Timer::timeScale(float newScale)
{
	mTimeScale = newScale;
}