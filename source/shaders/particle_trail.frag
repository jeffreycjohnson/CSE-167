#version 330

in float vDir;

uniform sampler2D inputTex;


layout(location = 0) out vec4 fragColor;

void main() {
	//vec3 color = texture(inputTex, vTexCoord).rgb;
	float strength = 1-abs(vDir);
    fragColor = vec4(1*5,5*strength, 5*strength*strength, 0.5*strength);
}
