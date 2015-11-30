#version 330
in vec3 eyeDir;

uniform samplerCube environment;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 eyeColor;

void main() {
    fragColor = texture(environment, normalize(eyeDir.xyz));
    eyeColor = vec4(.5*(eyeDir) + vec3(.5, .5, .5), 1.0);
}
