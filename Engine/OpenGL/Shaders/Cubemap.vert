#version 400 core
layout (location = 0) in vec3 in_Position;

out vec3 WorldPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    WorldPos = in_Position;
    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}