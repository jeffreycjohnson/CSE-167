#version 330
in vec2 vTexCoord;

uniform sampler2D inputTex;

layout(location = 0) out vec4 fragColor;

void main() {
	vec3 color = texture(inputTex, vTexCoord).rgb;
	//color *= 16;
	//color = color / (1 + color);
	//color = vec3(pow(color.x, 1/2.2), pow(color.y, 1/2.2), pow(color.z, 1/2.2));
    fragColor = vec4(color,1);
}
