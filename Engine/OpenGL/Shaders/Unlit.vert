#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texCoord;
layout (location = 2) in vec3 in_normal;

uniform mat4 in_projection;
uniform mat4 in_view;
uniform mat4 in_model;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

void main()
{
	TexCoords = in_texCoord;
    WorldPos = vec3(in_model * vec4(in_position, 1.0));
    Normal = mat3(in_model) * in_normal;   

    gl_Position =  in_projection * in_view * vec4(WorldPos, 1.0);
};

