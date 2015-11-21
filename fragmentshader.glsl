#version 330 core

uniform float time;
uniform sampler2D depthTex;

in vec3 interpolatedNormal;
in vec2 st;

out vec4 color;

void main() {
     vec4 texcolor = vec4(1.0, 1.0, 1.0, 1.0);
     vec3 nNormal = normalize(interpolatedNormal);
     float diffuse = max(0.0, nNormal.z);
     color = texcolor * diffuse;
}
