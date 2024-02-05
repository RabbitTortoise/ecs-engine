#version 330 core

uniform sampler2D textureMap;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

out vec4 FragColor;


void main()
{
	vec4 result =  texture2D(textureMap, TexCoords); 
	FragColor = result;
}