#version 330 core

uniform sampler2D image;
out vec4 FragColor;
in vec2 TexCoord;

int neighborCount (ivec2 c) {
    int count = 0;
    count += texelFetchOffset(image, c, 0, ivec2(0,1)).r > 0 ? 1 : 0;
    count += texelFetchOffset(image, c, 0, ivec2(0,-1)).r > 0 ? 1 : 0;
    count += texelFetchOffset(image, c, 0, ivec2(1,0)).r > 0 ? 1 : 0;
    count += texelFetchOffset(image, c, 0,  ivec2(1,1)).r > 0 ? 1 : 0;
    count += texelFetchOffset(image, c, 0,  ivec2(1,-1)).r > 0 ? 1 : 0;
    count += texelFetchOffset(image, c, 0, ivec2(-1,0)).r > 0 ? 1 : 0;
    count += texelFetchOffset(image, c, 0,  ivec2(-1,1)).r > 0 ? 1 : 0;
    count += texelFetchOffset(image, c, 0,  ivec2(-1,-1)).r > 0 ? 1 : 0;
    return count;
}

void main () {
    ivec2 v = ivec2(gl_FragCoord.xy);
    int neighbors = neighborCount(v);
    bool occupied = texelFetchOffset(image, v, 0, ivec2(0,0)).r > 0;
    if (!occupied) {
        if (neighbors == 3) FragColor = vec4(1,0,0,1);
        else FragColor = vec4(0);
    } else {
        if (neighbors == 2 || neighbors == 3) FragColor = vec4(1,0,0,1);
        else FragColor = vec4(0);
    }
}
