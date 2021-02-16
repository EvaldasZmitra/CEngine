#version 330 core

in vec2 uv;
in vec3 normal;
uniform sampler2D myTextureSampler;

out vec3 color;

void main(){
  float f = dot(normal, vec3(0.57735026919, 0.57735026919, 0.57735026919));
  color = max(f, 0.1) * texture( myTextureSampler, uv * 100).rgb;
}