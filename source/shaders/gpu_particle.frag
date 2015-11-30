#version 330

in float opacity;
in float angle;
in vec2 texCoord;
in vec3 color;

uniform sampler2D tex;

out vec4 frag_color;

void main()
{
	mat2 rot = mat2(cos(angle), sin(angle), -sin(angle), cos(angle));
	vec2 coord = (texCoord - 0.5f) * rot + 0.5;
	vec4 texel = texture(tex, coord);
	frag_color.rgba = texel * vec4(color, opacity);
}