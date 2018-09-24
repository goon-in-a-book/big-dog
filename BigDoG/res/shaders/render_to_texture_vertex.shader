#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;

void main()
{
	UV = vertexUV;
	gl_Position = position;
}