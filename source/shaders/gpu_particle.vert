#version 330

layout (location = 0) in float startTime;
layout (location = 1) in float duration;
layout (location = 2) in int corner; // 0 = BL, 1 = BR, 2 = TL, 3 = TR
layout (location = 3) in uint seed;
layout (location = 4) in vec3 startPos;
layout (location = 5) in vec3 startVelocity;
// Add more

uniform mat4 uM_Matrix;
uniform mat4 uV_Matrix;
uniform mat4 uP_Matrix;
uniform float elapsedTime;
uniform float deltaTime;
uniform vec3 minVelocity;
uniform vec3 maxVelocity;

out float opacity;
out vec2 texCoord;

vec4 pos;
vec3 velocity;
float size = 0.025f;

uint rand();
float range(float min, float max);
void srand(uint newSeed);

void main()
{
	srand(seed + uint(elapsedTime) / uint(duration));
	velocity = minVelocity + (maxVelocity - minVelocity) 
		* vec3(float(rand()) / 4294967295f, float(rand()) / 4294967295f, float(rand()) / 4294967295f);

	//size = range(0.1f, 0.5f);

	texCoord.x = (corner == 1 || corner == 2) ? 1 : 0;
	texCoord.y = corner > 1 ? 1 : 0;
	
	pos = vec4(startPos, 1);
	float t = mod(elapsedTime + startTime, duration);
	pos.xyz += velocity * t + 0.5f * vec3(0, -30, 0) * t * t;

	// Billboard particles
	pos = uV_Matrix * uM_Matrix * pos;
	pos.x += (texCoord.x - 0.5f) * size; // Multiply by size
	pos.y += (texCoord.y - 0.5f) * size;

	opacity = 1 - t / duration;

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
	return min + (max - min) * rand() / float(4294967295);
}

void srand(uint newSeed)
{
	rSeed = newSeed;
}