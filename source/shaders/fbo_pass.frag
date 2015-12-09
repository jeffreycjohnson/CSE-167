#version 330
in vec2 vTexCoord;

uniform sampler2D inputTex;

layout(location = 0) out vec4 fragColor;

void main() {
	vec3 color = texture(inputTex, vTexCoord).rgb;
	if (color.r  > 0) color = color;
	else color = vec3(0);
	color = clamp(color, vec3(0.0), vec3(1000.0));
    fragColor = vec4(color,1);
}
