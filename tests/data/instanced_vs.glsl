#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 norm;
layout (location = 3) in vec2 offset;

out vec2 TexCoord;

void main(void) {
    gl_Position = vec4(position.xy + offset, 0, 1);
    TexCoord = uv;
}
