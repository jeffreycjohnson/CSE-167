#version 330
in vec2 vTexCoord;

uniform sampler2D inputTex;

layout(location = 0) out vec4 fragColor;

uniform float exposure = 0.5;

void main() {
	vec3 color = texture(inputTex, vTexCoord).rgb;
	color *= exposure;
	color = color / (color + vec3(1));
	color = pow(color, vec3(1/2.2));
    fragColor = vec4(color,1);
}
