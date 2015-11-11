#version 150
precision mediump float;
in vec3 vNormal;
in vec2 vTexCoord;

out vec4 frag_color;

uniform sampler2D colorTex;

void main () {
  //vec3 color = 0.5*normalize(vNormal) + vec3(0.5,0.5,0.5);
  vec3 color = texture(colorTex, vTexCoord).rgb * vec3(0.5, 0.5, 0.9);
  frag_color = vec4(color, 1.0);
}
