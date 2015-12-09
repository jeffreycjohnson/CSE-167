#version 330
in vec2 vTexCoord;

uniform sampler2D inputTex;
uniform sampler2D addTex;

uniform float exposure = 0.5f;

layout(location = 0) out vec4 fragColor;

void main() {
	vec3 color = textureLod(inputTex, vTexCoord, 0).rgb + max(texture(addTex, vTexCoord).rgb - 0.5, vec3(0.0));
	float exposureTex = length(textureLod(inputTex, vTexCoord, 10.f))*10;
	color *= mix(exposure, exposureTex, 0.25);
	color = color / (color + vec3(1));
	color = pow(color, vec3(1/2.2));
    fragColor = vec4(color,1);
}
