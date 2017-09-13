#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

void main () {
    FragColor = vec4(TexCoord,0,1);
}
