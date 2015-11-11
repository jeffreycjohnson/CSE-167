#version 330
precision mediump float;
in vec3 vNormal;
in vec2 vTexCoord;

layout(location = 0) out vec4 frag_color;
layout(location = 1) out vec4 normal;

uniform sampler2D colorTex;

void main () {
  vec3 normalColor = 0.5*normalize(vNormal) + vec3(0.5,0.5,0.5);
  vec3 color = texture(colorTex, vTexCoord).rgb * vec3(0.5, 0.5, 0.9);
  frag_color = vec4(color, 1.0);
  normal = vec4(normalColor, 1.0);
}
