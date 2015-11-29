#version 330

layout (location = 0) in float startTime;
layout (location = 1) in int corner; // 0 = BL, 1 = BR, 2 = TL, 3 = TR
layout (location = 2) in vec3 startPos;
layout (location = 3) in vec3 startVelocity;
// Add more

uniform mat4 uM_Matrix;
uniform mat4 uV_Matrix;
uniform mat4 uP_Matrix;
uniform float elapsedTime;

out float opacity;
out vec2 texCoord;

vec4 pos;

void main()
{
	opacity = 0;
	texCoord.x = corner % 2;
	texCoord.y = corner > 1 ? 1 : 0;
	pos = vec4(startPos, 1);
	pos.x += texCoord.x - 0.5;
	pos.y += texCoord.y - 0.5;
	gl_Position = uP_Matrix * uV_Matrix * uM_Matrix * pos;
}