#version 150
precision mediump float;
in vec3 vNormal;

out vec4 frag_color;

void main () {
  frag_color = vec4(0.5*normalize(vNormal) + vec3(0.5,0.5,0.5), 1.0);
}
