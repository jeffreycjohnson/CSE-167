#include "Sound.h"
#include "GameObject.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_dsp.h"
#include "fmod/fmod_errors.h"
#include <iostream>
#include "Input.h"

FMOD::System* Sound::system;
std::unordered_map<std::string, FMOD::Sound*> Sound::soundMap;
FMOD_RESULT Sound::result;

Sound::Sound(std::string soundName, bool playOnAwake, bool looping, float volume)
{
	name = soundName;
	this->volume = volume;
	this->looping = looping;
	playing = active = playOnAwake;

	result = system->playSound(soundMap[name], 0, true, &channel);
	channel->setVolume(volume);
	if (looping)
	{
		channel->setMode(FMOD_LOOP_NORMAL);
		channel->setLoopCount(-1);
	}
	else
	{
		channel->setMode(FMOD_LOOP_OFF);
	}
}

Sound::~Sound()
{
	
}

void Sound::update(float deltaTime)
{
	position = gameObject->transform.getWorldPosition(); // Needs to be in world position for audio
	velocity = position - prevPosition;
	FMOD_VECTOR pos = { position.x, position.y, position.z };
	FMOD_VECTOR vel = { velocity.x, velocity.y, velocity.z };
	channel->set3DAttributes(&pos, &vel, 0);
	
	prevPosition = gameObject->transform.position;

	channel->setPaused(!playing); // Used at the end of update to prevent inconsistent initial volume
}

void Sound::play()
{
	if (playing)
	{
		// Possible leak, does FMOD handle deleting sound instances for playSound?
		result = system->playSound(soundMap[name], 0, true, &channel);
		channel->setVolume(volume);
		if (looping)
		{
			channel->setMode(FMOD_LOOP_NORMAL);
			channel->setLoopCount(-1);
		}
		else
		{
			channel->setMode(FMOD_LOOP_OFF);
		}
	}
	else // Paused
	{
		playing = true;
	}
	active = true;
}

void Sound::pause()
{
	playing = false;
}

void Sound::stop()
{
	channel->stop();
	active = false;
}

void Sound::toggle()
{
	if (active)
		stop();
	else
		play();
}

void Sound::setLooping(bool looping, int count = -1)
{
	if (looping)
		channel->setMode(FMOD_LOOP_NORMAL);
	else
		channel->setMode(FMOD_LOOP_OFF);

	channel->setLoopCount(count);
}

void Sound::setVolume(float volume)
{
	channel->setVolume(volume);
}

void Sound::init()
{
	result = FMOD::System_Create(&system);
	if (result != FMOD_OK)
	{
		std::cout << "FMOD Error " << result << std::endl;
		throw;
	}

	int driverCount = 0;
	result = system->getNumDrivers(&driverCount);

	if (driverCount == 0 || result != FMOD_OK)
	{
		std::cout << "FMOD Error " << result << std::endl;
		throw;
	}

	// Initialize our Instance with 36 Channels
	system->init(36, FMOD_INIT_NORMAL, NULL);

	// Generate sound map
	SoundClass cabin;
	system->createSound("assets/sounds/ambience/cabin.wav", FMOD_2D, NULL, &cabin);
	soundMap.insert({ "cabin", cabin });
	SoundClass laser;
	system->createSound("assets/sounds/laser.mp3", FMOD_3D, NULL, &laser);
	soundMap.insert({ "laser", laser });
	SoundClass gun;
	system->createSound("assets/sounds/gun.wav", FMOD_2D, NULL, &gun);
	soundMap.insert({ "gun", gun });
	// Add more sounds as we need
}

void Sound::updateFMOD()
{
	system->update();
}