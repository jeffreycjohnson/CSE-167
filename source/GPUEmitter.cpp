#include "GPUEmitter.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Timer.h"

using namespace std;

GPUEmitter::GPUEmitter(GameObject* go, string tex)
{
	gameObject = go;
	texture = new Texture(tex);
	shader = new Shader("source/shaders/gpu_particle.vert", "source/shaders/gpu_particle.frag");
	elapsedTimeUniform = glGetUniformLocation(shader->id, "elapsedTime");

	prevPosition = gameObject->transform.position;
	velocity = { 0, 0, 0 };
	minStartSize = maxStartSize = minEndSize = maxEndSize = 1;
	startOpacity = endOpacity = minDuration = maxDuration = 1;
	startColor = endColor = { 1, 1, 1 };
	minStartVelocity = { -1, -1, -1 };
	maxStartVelocity = { 1, 1, 1 };
	minAcceleration = maxAcceleration = { 0, 0, 0 };
	burst = trigger = false;
	count = 10;
	enabled = false;
}

GPUEmitter::~GPUEmitter()
{
	//delete texture;
	delete startTimes;
	delete quadCorners;
	delete startPositions;
	delete startVelocities;
}

void GPUEmitter::update(float deltaTime)
{
	glUniform1f(elapsedTimeUniform, Timer::time());
}

void GPUEmitter::draw()
{
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	texture->bindTexture(0);
	Renderer::switchShader(EMITTER_SHADER);

	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, count);
	glDisable(GL_BLEND);
	glDisable(GL_PROGRAM_POINT_SIZE);
}

void GPUEmitter::start()
{
	genParticles();
	enabled = true;
}

void GPUEmitter::restart()
{
	delete startTimes;
	delete quadCorners;
	delete startPositions;
	delete startVelocities;
	start();
}

GLuint GPUEmitter::genParticles()
{
	startTimes = new float[count * 4];
	quadCorners = new int[count * 4];
	startPositions = new float[count * 12];
	startVelocities = new float[count * 12];

	for (int i = 0; i < count - 3; i += 4)
	{
		startTimes[i] = startTimes[i + 1] = startTimes[i + 2] = startTimes[i + 3] = (float) i / count + Timer::time();
		quadCorners[i] = i % 4;
		quadCorners[i + 1] = (i + 1) % 4;
		quadCorners[i + 2] = (i + 2) % 4;
		quadCorners[i + 3] = (i + 3) % 4;
	}

	for (int i = 0; i < count * 3 - 3; i += 3)
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(startTimes), startTimes, GL_STATIC_DRAW);

	GLuint corner_vbo;
	glGenBuffers(1, &corner_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, corner_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadCorners), quadCorners, GL_STATIC_DRAW);

	GLuint position_vbo;
	glGenBuffers(1, &position_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(startPositions), startPositions, GL_STATIC_DRAW);

	GLuint velocity_vbo;
	glGenBuffers(1, &velocity_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, velocity_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(startVelocities), startVelocities, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, time_vbo);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, corner_vbo);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, velocity_vbo);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	return vao;
}

void GPUEmitter::setGameObject(GameObject* go)
{
	this->gameObject = go;
}