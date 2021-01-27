#version 330 core

in vec2 TexCoords;
out vec4 Color;

uniform sampler2D ng_texture;

void main(){
    Color=texture(ng_texture,TexCoords);
}