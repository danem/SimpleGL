
layout (std140) uniform ShaderParams {
    // Member                 base   aligned
    vec4 emitterColor;     // 16     0
    vec2 emitterPosition;  // 8      16
    float emitterRadius;   // 4      24
    float timeStep;        // 4      28
    float dissipation;     // 4      32
    float width;           // 4      36
    float height;          // 4      40
    float sign;            // 4      44
};


float linearScale (float rlo, float rhi, float dlo, float dhi, float x){
    float xa = x - rlo;
    float dr = rhi - rlo;
    float ratio = xa / dr;
    return (dlo * (1.0 - ratio)) + (dhi * ratio);
}

vec4 linearScale (float rlo, float rhi, float dlo, float dhi, vec4 v){
    return vec4(
        linearScale(rlo, rhi, dlo, dhi, v.x),
        linearScale(rlo, rhi, dlo, dhi, v.y),
        linearScale(rlo, rhi, dlo, dhi, v.z),
        linearScale(rlo, rhi, dlo, dhi, v.w)
    );
}


#if defined(EMITTER_SHADER)

in vec2 TexCoord;
out vec4 FragColor;

void main(void) {
    if (distance(TexCoord, emitterPosition) > emitterRadius) FragColor = vec4(0,0,0,1);
    FragColor = emitterColor;
}

#elif defined(ADVECT_SHADER)

uniform sampler2D Velocity;
uniform sampler2D Source;

in vec2 TexCoord;
out vec4 FragColor;


void main() {
    vec2 u = texture(Velocity, TexCoord).xy;
    vec2 coord = TexCoord - timeStep * u * sign;
    FragColor = dissipation * texture(Source, coord);
}

#elif defined(CONVOLVE_SHADER)

uniform sampler2D Source;
uniform mat3 weights;

in vec2 TexCoord;
out vec4 FragColor;

void main () {
    vec2 tx = 1.0 / vec2(width, height);
    FragColor = normalize(weights[0][0] * texture(Source, TexCoord - tx)
              + weights[1][0] * texture(Source, TexCoord + vec2(0,-tx.y))
              + weights[2][0] * texture(Source, TexCoord + vec2(tx.x,-tx.y))
              + weights[0][1] * texture(Source, TexCoord + vec2(-tx.x,0))
              + weights[1][1] * texture(Source, TexCoord)
              + weights[2][1] * texture(Source, TexCoord + vec2(tx.x, 0))
              + weights[0][2] * texture(Source, TexCoord + vec2(-tx.x,tx.y))
              + weights[1][2] * texture(Source, TexCoord + vec2(0,tx.y))
              + weights[2][2] * texture(Source, TexCoord + tx));
}

#elif defined(VISUALIZE_SHADER)

uniform sampler2D Source;

in vec2 TexCoord;
out vec4 FragColor;

void main() {
    FragColor = texture(Source, TexCoord);
}


#endif
