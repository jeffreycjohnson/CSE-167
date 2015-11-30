#version 430

layout (location = 1) in float duration;
layout (location = 2) in int corner; // 0 = BL, 1 = BR, 2 = TL, 3 = TR
layout (location = 3) in uint seed;

uniform mat4 uM_Matrix;
uniform mat4 uV_Matrix;
uniform mat4 uP_Matrix;

// Updates differently per version
uniform vec3 emitterPos;
uniform vec3 emitterVelocity;

uniform vec3 minVelocity;
uniform vec3 maxVelocity;
uniform vec3 minAcceleration;
uniform vec3 maxAcceleration;
uniform float elapsedTime;
uniform float minStartSize;
uniform float maxStartSize;
uniform float minEndSize;
uniform float maxEndSize;

// Burst specific
uniform float startTime;
uniform uint burstSeed;

out float opacity;
out vec3 color;
out vec2 texCoord;

uint rand();
float range(float min, float max);
void srand(uint newSeed);

void main()
{
	vec4 pos;
	vec3 tmpVec;
	float tmp, t;
	
	srand(seed + burstSeed);

	texCoord.x = int(corner == 1 || corner == 2);
	texCoord.y = int(corner > 1);
	
	// Init
	pos = vec4(emitterPos, 1);
	t = elapsedTime - startTime;

	// Velocity
	tmpVec = minVelocity + (maxVelocity - minVelocity) 
		* vec3(float(rand()) / 4294967295f, float(rand()) / 4294967295f, float(rand()) / 4294967295f);
	pos.xyz += (tmpVec + emitterVelocity * 20) * t;

	// Acceleration
	tmpVec = minAcceleration + (maxAcceleration - minAcceleration) 
		* vec3(float(rand()) / 4294967295f, float(rand()) / 4294967295f, float(rand()) / 4294967295f);
	pos.xyz += 0.5f * tmpVec * t * t;

	// Size
	tmp = range(minStartSize, maxStartSize);

	// Billboard particles
	pos = uV_Matrix * uM_Matrix * pos;
	pos.x += (texCoord.x - 0.5f) * tmp; // Multiply by size
	pos.y += (texCoord.y - 0.5f) * tmp;

	uint finished = 1u - min(1u, uint((elapsedTime - startTime) / duration));
	opacity = (1f - t / duration) * float(finished);

	gl_Position = uP_Matrix * pos;
}

// RANDOM FUNCTIONS
// Source: http://www.reedbeta.com/blog/2013/01/12/quick-and-easy-gpu-random-numbers-in-d3d11/

uint rSeed = 1u;

uint rand()
{
    rSeed = (rSeed ^ 61u) ^ (rSeed >> 16u);
    rSeed *= 9u;
    rSeed = rSeed ^ (rSeed >> 4u);
    rSeed *= 0x27d4eb2du;
    rSeed = rSeed ^ (rSeed >> 15u);
    return rSeed;
}

float range(float min, float max)
{
	return min + (max - min) * (rand() / 4294967295f);
}

void srand(uint newSeed)
{
	rSeed = newSeed;
}