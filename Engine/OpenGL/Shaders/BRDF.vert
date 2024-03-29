#version 400 core


layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TexCoord;

out vec2 TexCoords;

void main()
{
    TexCoords = in_TexCoord;
	gl_Position = vec4(in_Position, 1.0);
}