#include "GPUEmitter.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Timer.h"

using namespace std;

GPUEmitter::GPUEmitter(GameObject* go, string tex, bool burstEmitter)
{
	gameObject = go;
	texture = new Texture(tex);
	if (burstEmitter)
		Renderer::switchShader(EMITTER_BURST_SHADER);
	else
		Renderer::switchShader(EMITTER_SHADER);

	if (burstEmitter)
	{
		startTimeUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "startTime");
		burstSeedUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "burstSeed");
	}

	emitterPosUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "emitterPos");
	emitterVelocityUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "emitterVelocity");
	elapsedTimeUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "elapsedTime");
	minVelocityUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "minVelocity");
	maxVelocityUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "maxVelocity");
	minAccelUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "minAcceleration");
	maxAccelUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "maxAcceleration");
	minStartSizeUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "minStartSize");
	maxStartSizeUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "maxStartSize");
	minEndSizeUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "minEndSize");
	maxEndSizeUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "maxEndSize");
	minStartColorUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "minStartColor");
	maxStartColorUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "maxStartColor");
	minEndColorUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "minEndColor");
	maxEndColorUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "maxEndColor");
	startOpacityUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "startOpacity");
	endOpacityUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "endOpacity");
	minStartAngleUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "minStartAngle");
	maxStartAngleUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "maxStartAngle");
	minAngularVelocityUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "minAngularVelocity");
	maxAngularVelocityUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "maxAngularVelocity");

	prevPosition = gameObject->transform.position;
	velocity = { 0, 0, 0 };
	minStartSize = 0.2;
	maxStartSize = 0.5;
	minEndSize = 0.5;
	maxEndSize = 1;
	startOpacity = 1;
	endOpacity = 0;
	minDuration = 0;
	maxDuration = 3;
	minStartColor = { 1, 1, 1 };
	maxStartColor = { 1, 1, 1 };
	minEndColor = { 1, 1, 1 };
	maxEndColor = { 1, 1, 1 };
	minStartVelocity = { -5, -10, -5 };
	maxStartVelocity = { 10, 5, 10 };
	minAcceleration = { -30, 0, -30 };
	maxAcceleration = { 0, 30, 0 };
	minStartAngle = 0;
	maxStartAngle = 360;
	minAngularVelocity = -10;
	maxAngularVelocity = 10;
	emitterVelocityScale = 10;
	burst = burstEmitter;
	trigger = false;
	count = 2000;
	enabled = false;
	loop = false;
	additive = true;
	startTime = Timer::time();
}

GPUEmitter::~GPUEmitter()
{
	if (!burst)
		delete startTimes;

	delete durations;
	delete quadCorners;
	delete seeds;
}

void GPUEmitter::update(float deltaTime)
{
	if (burst)
		Renderer::switchShader(EMITTER_BURST_SHADER);
	else
		Renderer::switchShader(EMITTER_SHADER);

	glUniform1f(elapsedTimeUniform, (GLfloat)Timer::time());

	if (burst)
	{
		if (loop && (Timer::time() - startTime) > maxDuration)
			play();

		if (trigger)
		{
			glUniform1f(startTimeUniform, (GLfloat)startTime);
			glUniform1ui(burstSeedUniform, (GLuint)rand());
			glUniform3f(emitterPosUniform, gameObject->transform.position.x,
				gameObject->transform.position.y,
				gameObject->transform.position.z);
			glUniform3f(emitterVelocityUniform, (gameObject->transform.position.x - prevPosition.x) * emitterVelocityScale,
				(gameObject->transform.position.y - prevPosition.y) * emitterVelocityScale,
				(gameObject->transform.position.z - prevPosition.z) * emitterVelocityScale);
			trigger = false;
		}
	}
	else
	{
		glUniform3f(emitterPosUniform, gameObject->transform.position.x,
			gameObject->transform.position.y,
			gameObject->transform.position.z);
		glUniform3f(emitterVelocityUniform, (gameObject->transform.position.x - prevPosition.x) * emitterVelocityScale,
			(gameObject->transform.position.y - prevPosition.y) * emitterVelocityScale,
			(gameObject->transform.position.z - prevPosition.z) * emitterVelocityScale);
	}

	prevPosition = gameObject->transform.position;
}

void GPUEmitter::draw()
{
	if (enabled)
	{
		glEnable(GL_BLEND);
		if (additive)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		else
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(false);
		texture->bindTexture(0);

		if (burst)
			Renderer::switchShader(EMITTER_BURST_SHADER);
		else
			Renderer::switchShader(EMITTER_SHADER);

		glBindVertexArray(vao);
		glDrawArrays(GL_QUADS, 0, count * 4);
		glBindVertexArray(NULL);
		glDepthMask(true);
		glDisable(GL_BLEND);
	}
}

void GPUEmitter::init()
{
	genParticles();
}

void GPUEmitter::restart()
{
	if (!burst)
		delete startTimes;

	delete durations;
	delete quadCorners;
	delete seeds;
	init();
}

void GPUEmitter::play()
{
	trigger = true;
	enabled = true;
	startTime = Timer::time();
}

GLuint GPUEmitter::genParticles()
{
	if (burst)
		Renderer::switchShader(EMITTER_BURST_SHADER);
	else
	{
		Renderer::switchShader(EMITTER_SHADER);
		startTimes = new float[count * 4];
	}

	durations = new float[count * 4];
	quadCorners = new int[count * 4];
	seeds = new unsigned int[count * 4];

	srand(Timer::time());

	for (int i = 0; i < count * 4 - 3; i += 4)
	{
		if (!burst)
			startTimes[i] = startTimes[i + 1] = startTimes[i + 2] = startTimes[i + 3] = (float) i / count + Timer::time();

		seeds[i] = seeds[i + 1] = seeds[i + 2] = seeds[i + 3] = rand();
		quadCorners[i] = i % 4;
		quadCorners[i + 1] = (i + 1) % 4;
		quadCorners[i + 2] = (i + 2) % 4;
		quadCorners[i + 3] = (i + 3) % 4;

		float rnd = minDuration + (((maxDuration - minDuration) * rand()) / (RAND_MAX + 1.0f));
		durations[i] = durations[i + 1] = durations[i + 2] = durations[i + 3] = rnd;
	}

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

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	if (!burst)
	{
		GLuint time_vbo;
		glGenBuffers(1, &time_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, time_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(startTimes[0]) * count * 4, startTimes, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, time_vbo);
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
	}
	glBindBuffer(GL_ARRAY_BUFFER, duration_vbo);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, corner_vbo);
	glVertexAttribIPointer(2, 1, GL_INT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, seed_vbo);
	glVertexAttribIPointer(3, 1, GL_INT, 0, NULL);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glBindVertexArray(NULL);

	if (burst)
	{
		glUniform1f(startTimeUniform, (GLfloat) 0);
		glUniform1ui(burstSeedUniform, (GLuint) rand());
	}

	glUniform3f(emitterPosUniform, gameObject->transform.position.x, 
		gameObject->transform.position.y, gameObject->transform.position.z);
	glUniform3f(emitterVelocityUniform, 0, 0, 0);
	glUniform3f(minVelocityUniform, minStartVelocity.x, minStartVelocity.y, minStartVelocity.z);
	glUniform3f(maxVelocityUniform, maxStartVelocity.x, maxStartVelocity.y, maxStartVelocity.z);
	glUniform3f(minAccelUniform, minAcceleration.x, minAcceleration.y, minAcceleration.z);
	glUniform3f(maxAccelUniform, maxAcceleration.x, maxAcceleration.y, maxAcceleration.z);
	glUniform1f(minStartSizeUniform, minStartSize);
	glUniform1f(maxStartSizeUniform, maxStartSize);
	glUniform1f(minEndSizeUniform, minEndSize);
	glUniform1f(maxEndSizeUniform, maxEndSize);
	glUniform3f(minStartColorUniform, minStartColor.x, minStartColor.y, minStartColor.z);
	glUniform3f(maxStartColorUniform, maxStartColor.x, maxStartColor.y, maxStartColor.z);
	glUniform3f(minEndColorUniform, minEndColor.x, minEndColor.y, minEndColor.z);
	glUniform3f(maxEndColorUniform, maxEndColor.x, maxEndColor.y, maxEndColor.z);
	glUniform1f(startOpacityUniform, startOpacity);
	glUniform1f(endOpacityUniform, endOpacity);
	glUniform1f(minStartAngleUniform, minStartAngle);
	glUniform1f(maxStartAngleUniform, maxStartAngle);
	glUniform1f(minAngularVelocityUniform, minAngularVelocity);
	glUniform1f(maxAngularVelocityUniform, maxAngularVelocity);

	return vao;
}

void GPUEmitter::setGameObject(GameObject* go)
{
	this->gameObject = go;
}