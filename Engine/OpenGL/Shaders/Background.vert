#version 400 core
layout (location = 0) in vec3 in_Position;

uniform mat4 in_Projection;
uniform mat4 in_View;

out vec3 localPos;

void main()
{
    localPos = in_Position;

    mat4 rotView = mat4(mat3(in_View)); // remove translation from the view matrix
    vec4 clipPos = in_Projection * rotView * vec4(localPos, 1.0);

    gl_Position = clipPos.xyww;
}