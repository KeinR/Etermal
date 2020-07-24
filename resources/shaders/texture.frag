#version 330 core
out vec4 FragColor;

in vec2 texturePos;

uniform sampler2D Tex;

void main()
{
	FragColor = vec4(texture(Tex, texturePos).rgb, 1.0f);
}
