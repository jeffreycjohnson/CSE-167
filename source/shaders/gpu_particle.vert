#version 330

layout (location = 0) in float startTime;
layout (location = 1) in float duration;
layout (location = 2) in int corner; // 0 = BL, 1 = BR, 2 = TL, 3 = TR
layout (location = 3) in int seed;
layout (location = 4) in vec3 startPos;
layout (location = 5) in vec3 startVelocity;
// Add more

uniform mat4 uM_Matrix;
uniform mat4 uV_Matrix;
uniform mat4 uP_Matrix;
uniform float elapsedTime;
uniform float deltaTime;

out float opacity;
out vec2 texCoord;

vec4 pos;
float size = 0.5f;

uint rand();
float range(float min, float max);
void srand(uint newSeed);

void main()
{
	opacity = 0.5;
	texCoord.x = (corner == 1 || corner == 2) ? 1 : 0;
	texCoord.y = corner > 1 ? 1 : 0;
	
	pos = vec4(startPos, 1);
	pos.y = mod(elapsedTime + startTime, duration);

	// Billboard particles
	pos = uV_Matrix * uM_Matrix * pos;
	pos.x += (texCoord.x - 0.5f) * size; // Multiply by size
	pos.y += (texCoord.y - 0.5f) * size;

	gl_Position = uP_Matrix * pos;
}

// RANDOM FUNCTIONS
// Source: http://www.reedbeta.com/blog/2013/01/12/quick-and-easy-gpu-random-numbers-in-d3d11/

uint rSeed = 0u;

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
	float percent = rand() / float(4294967295);
	return percent;
}

void srand(uint newSeed)
{
	rSeed = newSeed;
}