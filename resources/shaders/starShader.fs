#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texturestar;


void main()
{

    FragColor = texture(texturestar, TexCoords) * vec4(1.0);
}