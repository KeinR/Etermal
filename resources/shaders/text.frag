#version 330 core
out vec4 FragColor;

in vec2 texturePos;

uniform vec3 backgroundColor;
uniform vec3 foregroundColor;

uniform sampler2D Tex;

void main()
{
	float alpha = float(texture(Tex, texturePos));
	FragColor = vec4(backgroundColor * vec3(1 - alpha) + foregroundColor * vec3(alpha), alpha);
}
