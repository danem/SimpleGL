#version 330 core

uniform mat4 cameraMat;
uniform mat4 viewMat;
//uniform mat4 modelMat;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uvcoord;

out vec2 TexCoord;

void main () {
    TexCoord = uvcoord;
    gl_Position = cameraMat * viewMat *  vec4(position, 1.0f);
}
