#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 2) in vec2 uv_in;

uniform mat4 VP;
out vec2 uv;

void main(){
    vec4 p = VP * vec4(vertex, 1);
    gl_Position = p;
    uv = vec2(
        (vertex.x + 1) / 2, 
        (vertex.y + 1) / 2
    );
}