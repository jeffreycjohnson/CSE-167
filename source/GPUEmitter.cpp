#include "GPUEmitter.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Timer.h"

using namespace std;

GPUEmitter::GPUEmitter(GameObject* go, string tex)
{
	gameObject = go;
	texture = new Texture(tex);
	Renderer::switchShader(EMITTER_SHADER);
	elapsedTimeUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "elapsedTime");
	deltaTimeUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "deltaTime");

	prevPosition = gameObject->transform.position;
	velocity = { 0, 0, 0 };
	minStartSize = maxStartSize = minEndSize = maxEndSize = 1;
	startOpacity = endOpacity = 1;
	minDuration = 0.1;
	maxDuration = 2;
	startColor = endColor = { 1, 1, 1 };
	minStartVelocity = { -1, -1, -1 };
	maxStartVelocity = { 1, 1, 1 };
	minAcceleration = maxAcceleration = { 0, 0, 0 };
	burst = trigger = false;
	count = 50000;
	enabled = false;
}

GPUEmitter::~GPUEmitter()
{
	//delete texture;
	delete startTimes;
	delete durations;
	delete quadCorners;
	delete startPositions;
	delete startVelocities;
	delete seeds;
}

void GPUEmitter::update(float deltaTime)
{
	Renderer::switchShader(EMITTER_SHADER);
	glUniform1f(elapsedTimeUniform, (GLfloat) Timer::time());
	glUniform1f(deltaTimeUniform, (GLfloat) deltaTime);
}

void GPUEmitter::draw()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	texture->bindTexture(0);
	Renderer::switchShader(EMITTER_SHADER);

	glBindVertexArray(vao);
	glDrawArrays(GL_QUADS, 0, count * 4);
	glBindVertexArray(NULL);
	glDisable(GL_BLEND);
}

void GPUEmitter::start()
{
	genParticles();
	enabled = true;
}

void GPUEmitter::restart()
{
	delete startTimes;
	delete durations;
	delete quadCorners;
	delete startPositions;
	delete startVelocities;
	delete seeds;
	start();
}

GLuint GPUEmitter::genParticles()
{
	startTimes = new float[count * 4];
	durations = new float[count * 4];
	quadCorners = new int[count * 4];
	startPositions = new float[count * 12];
	startVelocities = new float[count * 12];
	seeds = new unsigned int[count * 4];

	srand(Timer::time());

	for (int i = 0; i < count * 4 - 3; i += 4)
	{
		startTimes[i] = startTimes[i + 1] = startTimes[i + 2] = startTimes[i + 3] = (float) i / count + Timer::time();
		seeds[i] = seeds[i + 1] = seeds[i + 2] = seeds[i + 3] = rand();
		quadCorners[i] = i % 4;
		quadCorners[i + 1] = (i + 1) % 4;
		quadCorners[i + 2] = (i + 2) % 4;
		quadCorners[i + 3] = (i + 3) % 4;

		float rnd = minDuration + (((maxDuration - minDuration) * rand()) / (RAND_MAX + 1.0f));
		durations[i] = durations[i + 1] = durations[i + 2] = durations[i + 3] = rnd;
	}

	for (int i = 0; i < count * 4 * 3 - 2; i += 3)
	{
		startPositions[i] = 0;
		startPositions[i + 1] = 0;
		startPositions[i + 2] = 0;

		startVelocities[i] = 0;
		startVelocities[i + 1] = 0;
		startVelocities[i + 2] = 0;
	}

	GLuint time_vbo;
	glGenBuffers(1, &time_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, time_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(startTimes[0]) * count * 4, startTimes, GL_STATIC_DRAW);

	GLuint duration_vbo;
	glGenBuffers(1, &duration_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, duration_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(durations[0]) * count * 4, durations, GL_STATIC_DRAW);

	GLuint corner_vbo;
	glGenBuffers(1, &corner_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, corner_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadCorners[0]) * count * 4, quadCorners, GL_STATIC_DRAW);

	GLuint seed_vbo;
	glGenBuffers(1, &seed_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, seed_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(seeds[0]) * count * 4, seeds, GL_STATIC_DRAW);

	GLuint position_vbo;
	glGenBuffers(1, &position_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(startPositions[0]) * count * 12, startPositions, GL_STATIC_DRAW);

	GLuint velocity_vbo;
	glGenBuffers(1, &velocity_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, velocity_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(startVelocities[0]) * count * 12, startVelocities, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, time_vbo);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, duration_vbo);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, corner_vbo);
	glVertexAttribIPointer(2, 1, GL_INT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, seed_vbo);
	glVertexAttribIPointer(3, 1, GL_INT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, velocity_vbo);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glBindVertexArray(NULL);

	return vao;
}

void GPUEmitter::setGameObject(GameObject* go)
{
	this->gameObject = go;
}