#version 330

in float opacity;
in vec2 texCoord;

uniform sampler2D tex;

out vec4 frag_color;

void main()
{
	vec4 texel = texture(tex, texCoord);
	frag_color.rgba = texel * vec4(1, 1, 1, opacity);
}