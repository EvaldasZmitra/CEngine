#version 330 core
layout(location = 0) in vec3 vertex;

uniform mat4 VP;

void main(){
    vec4 p = VP * vec4(vertex, 1);
    gl_Position = p;
}