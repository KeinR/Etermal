#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 texPos;

uniform mat4 Model;

out vec2 texturePos;

void main()
{
    texturePos = texPos;
    gl_Position = Model * vec4(aPos, 0.0, 1.0);
}
