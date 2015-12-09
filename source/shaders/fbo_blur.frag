#version 330
in vec2 vTexCoord;

uniform sampler2D inputTex;

uniform float width;
uniform float height;
uniform vec2 direction;
layout(location = 0) out vec4 fragColor;

uniform float val [7] = float[7](0.04, 0.111, 0.215, 0.268, 0.215, 0.111, 0.04);

void main() {
	vec3 color = vec3(0,0,0);
	for (int i=-3; i<=3; ++i) {
		color += val[i+3] * texture(inputTex, vTexCoord + (float(i) * direction / vec2(width, height))).rgb;
	}
    fragColor = vec4(color, 1.0);
}
