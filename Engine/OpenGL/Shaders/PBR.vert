#version 400 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TexCoord;
layout (location = 2) in vec3 in_Normal;
layout (location = 3) in vec3 in_Tangent;

uniform mat4 in_Projection;
uniform mat4 in_View;
uniform mat4 in_Model;


out VS_OUT {
    vec4 FragPos;
    vec2 TexCoords;
    vec3 T;
    vec3 B;
    vec3 N;
} vs_out;


void main()
{
    vec4 pos = in_Model * vec4(in_Position, 1.0);
    
    vec3 T = normalize(vec3(in_Model * vec4(in_Tangent, 0.0)));
    vec3 N = normalize(vec3(in_Model * vec4(in_Normal, 0.0)));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    vs_out.T = T;
    vs_out.B = B;
    vs_out.N = N;

    vs_out.FragPos = pos;
    vs_out.TexCoords = in_TexCoord;   

    gl_Position = in_Projection * in_View * in_Model * vec4(in_Position, 1.0);
};

