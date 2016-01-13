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
	rotateTowardsVelocityUniform = glGetUniformLocation(Renderer::getCurrentShader().id, "rotateTowardsVelocity");

	prevPosition = gameObject->transform.getWorldPosition();
	velocity = { 0, 0, 0 };
	minStartSize = 0.4;
	maxStartSize = 0.6;
	minEndSize = 0.6;
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
	emitterVelocity = { 0, 0, 0 };
	emitterVelocityScale = 2;
	burst = burstEmitter;
	trigger = false;
	count = 4000;
	enabled = false;
	loop = false;
	additive = true;
	rotateTowardsVelocity = true;
	startTime = Timer::time();
}

GPUEmitter::~GPUEmitter()
{
	//delete texture;
	// Delete for arrays is handled in genParticles
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
			burstSeed = rand();
			burstStartPos = { gameObject->transform.getWorldPosition().x,
				gameObject->transform.getWorldPosition().y,
				gameObject->transform.getWorldPosition().z };
			trigger = false;
		}
	}

	prevPosition = gameObject->transform.getWorldPosition();
}

void GPUEmitter::draw()
{
	if (enabled)
	{
		if (burst)
			Renderer::switchShader(EMITTER_BURST_SHADER);
		else
			Renderer::switchShader(EMITTER_SHADER);

		if (burst)
		{
			glUniform1f(startTimeUniform, (GLfloat)startTime);
			glUniform1ui(burstSeedUniform, (GLuint)burstSeed);
			glUniform3f(emitterPosUniform, burstStartPos.x, burstStartPos.y, burstStartPos.z);
			glUniform3f(emitterVelocityUniform, emitterVelocity.x * emitterVelocityScale, 
												emitterVelocity.y * emitterVelocityScale, 
												emitterVelocity.z * emitterVelocityScale);
		}
		else
		{
			glUniform3f(emitterPosUniform, gameObject->transform.getWorldPosition().x,
				gameObject->transform.getWorldPosition().y,
				gameObject->transform.getWorldPosition().z);
			glUniform3f(emitterVelocityUniform, (gameObject->transform.getWorldPosition().x - prevPosition.x) * emitterVelocityScale,
				(gameObject->transform.getWorldPosition().y - prevPosition.y) * emitterVelocityScale,
				(gameObject->transform.getWorldPosition().z - prevPosition.z) * emitterVelocityScale);
		}
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
		unsigned int rotateVel = rotateTowardsVelocity ? 1 : 0;
		glUniform1ui(rotateTowardsVelocityUniform, rotateVel);

		glEnable(GL_BLEND);
		if (additive)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		else
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(false);
		texture->bindTexture(0);

		Renderer::setModelMatrix(glm::mat4(1));
		if (Renderer::gpuData.vaoHandle != vao) {
			glBindVertexArray(vao);
			Renderer::gpuData.vaoHandle = vao;
		}
		glDrawArrays(GL_QUADS, 0, count * 4);
		glBindVertexArray(NULL);
		glDepthMask(true);
		glDisable(GL_BLEND);
	}
}

void GPUEmitter::init()
{
	genParticles();
	if (!burst)
		enabled = true;
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

	glUniform3f(emitterPosUniform, gameObject->transform.getWorldPosition().x,
		gameObject->transform.getWorldPosition().y, gameObject->transform.getWorldPosition().z);
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
	unsigned int rotateVel = rotateTowardsVelocity ? 1 : 0;
	glUniform1ui(rotateTowardsVelocityUniform, rotateVel);

	// We don't need the arrays anymore, since we already passed them to the GPU
	if (!burst)
		delete[] startTimes;

	delete[] durations;
	delete[] quadCorners;
	delete[] seeds;

	return vao;
}
