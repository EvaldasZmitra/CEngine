#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 uv_in;

uniform mat4 VP;
uniform mat4 M;

out vec2 uv;
out vec3 normal;

void main(){
    vec4 p = VP * vec4(vertex, 1);
    gl_Position = p;
    uv = uv_in;
    normal = (M * vec4(in_normal, 0)).xyz;
}