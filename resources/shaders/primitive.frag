#version 330 core
out vec4 FragColor;

uniform vec4 Color;

void main()
{
	FragColor = Color;
	// FragColor = vec4(0, 1, 0, 1);
}
