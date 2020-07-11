#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 Model;

void main()
{
	gl_Position = Model * vec4(aPos, 0.0, 1.0);
}
