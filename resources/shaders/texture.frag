#version 330 core
out vec4 FragColor;

in vec2 texturePos;

// uniform vec3 foregroundColor;
// uniform vec3 backgroundColor;

uniform sampler2D Tex;

void main()
{
	FragColor = texture(Tex, texturePos);

	// vec4 Color = texture(Tex, texturePos);
	// if (Color.w == 0) {
	// 	FragColor = vec4(backgroundColor, Color.w);
	// } else {
	// 	FragColor = vec4(foregroundColor, Color.w);
	// }
}
