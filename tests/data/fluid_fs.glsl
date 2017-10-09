#if defined(SUBTRACT_SHADER)
    out vec2 FragColor;

    uniform sampler2D Velocity;
    uniform sampler2D Pressure;
    uniform sampler2D Obstacles;
    uniform float GradientScale;

    void main() {
        ivec2 T = ivec2(gl_FragCoord.xy);

        vec3 oC = texelFetch(Obstacles, T, 0).xyz;
        if (oC.x > 0) {
            FragColor = oC.yz;
            return;
        }

        // Find neighboring pressure:
        float pN = texelFetchOffset(Pressure, T, 0, ivec2(0, 1)).r;
        float pS = texelFetchOffset(Pressure, T, 0, ivec2(0, -1)).r;
        float pE = texelFetchOffset(Pressure, T, 0, ivec2(1, 0)).r;
        float pW = texelFetchOffset(Pressure, T, 0, ivec2(-1, 0)).r;
        float pC = texelFetch(Pressure, T, 0).r;

        // Find neighboring obstacles:
        vec3 oN = texelFetchOffset(Obstacles, T, 0, ivec2(0, 1)).xyz;
        vec3 oS = texelFetchOffset(Obstacles, T, 0, ivec2(0, -1)).xyz;
        vec3 oE = texelFetchOffset(Obstacles, T, 0, ivec2(1, 0)).xyz;
        vec3 oW = texelFetchOffset(Obstacles, T, 0, ivec2(-1, 0)).xyz;

        // Use center pressure for solid cells:
        vec2 obstV = vec2(0);
        vec2 vMask = vec2(1);

        if (oN.x > 0) { pN = pC; obstV.y = oN.z; vMask.y = 0; }
        if (oS.x > 0) { pS = pC; obstV.y = oS.z; vMask.y = 0; }
        if (oE.x > 0) { pE = pC; obstV.x = oE.y; vMask.x = 0; }
        if (oW.x > 0) { pW = pC; obstV.x = oW.y; vMask.x = 0; }

        // Enforce the free-slip boundary condition:
        vec2 oldV = texelFetch(Velocity, T, 0).xy;
        vec2 grad = vec2(pE - pW, pN - pS) * GradientScale;
        vec2 newV = oldV - grad;
        FragColor = (vMask * newV) + obstV;
    }
#elif defined(JACOBI_SHADER)
    out vec4 FragColor;

    uniform sampler2D Pressure;
    uniform sampler2D Divergence;
    uniform sampler2D Obstacles;

    uniform float Alpha;
    uniform float InverseBeta;

    void main()
    {
        ivec2 T = ivec2(gl_FragCoord.xy);

        // Find neighboring pressure:
        vec4 pN = texelFetchOffset(Pressure, T, 0, ivec2(0, 1));
        vec4 pS = texelFetchOffset(Pressure, T, 0, ivec2(0, -1));
        vec4 pE = texelFetchOffset(Pressure, T, 0, ivec2(1, 0));
        vec4 pW = texelFetchOffset(Pressure, T, 0, ivec2(-1, 0));
        vec4 pC = texelFetch(Pressure, T, 0);

        // Find neighboring obstacles:
        vec3 oN = texelFetchOffset(Obstacles, T, 0, ivec2(0, 1)).xyz;
        vec3 oS = texelFetchOffset(Obstacles, T, 0, ivec2(0, -1)).xyz;
        vec3 oE = texelFetchOffset(Obstacles, T, 0, ivec2(1, 0)).xyz;
        vec3 oW = texelFetchOffset(Obstacles, T, 0, ivec2(-1, 0)).xyz;

        // Use center pressure for solid cells:
        if (oN.x > 0) pN = pC;
        if (oS.x > 0) pS = pC;
        if (oE.x > 0) pE = pC;
        if (oW.x > 0) pW = pC;

        vec4 bC = texelFetch(Divergence, T, 0);
        FragColor = (pW + pE + pS + pN + Alpha * bC) * InverseBeta;
    }

#elif defined(ADVECT_SHADER)
    out vec4 FragColor;

    uniform sampler2D velocityTexture;
    uniform sampler2D sourceTexture;
    uniform sampler2D obstacleTexture;

    uniform vec2 inverseSize;
    uniform float timeStep;
    uniform float dissipation;

    in vec2 TexCoord;

    void main()
    {
        vec2 fragCoord = gl_FragCoord.xy;
        float solid = texture(obstacleTexture, inverseSize * fragCoord).x;
        if (solid > 0) {
            FragColor = vec4(0);
            return;
        }

        vec2 u = texture(velocityTexture, inverseSize * fragCoord).xy;
        vec2 coord = inverseSize * (fragCoord - timeStep * u);
        FragColor = dissipation * texture(sourceTexture, coord);
    }

#elif defined(BOUYANCY_SHADER)
    out vec2 FragColor;
    uniform sampler2D velocityTexture;
    uniform sampler2D tempTexture;
    uniform sampler2D densityTexture;
    uniform float ambientTemperature;
    uniform float timeStep;
    uniform float sigma;
    uniform float kappa;

    void main()
    {
        ivec2 TC = ivec2(gl_FragCoord.xy);
        float T = texelFetch(tempTexture, TC, 0).r;
        vec2 V = texelFetch(velocityTexture, TC, 0).xy;

        FragColor = V;

        if (T > ambientTemperature) {
            float D = texelFetch(densityTexture, TC, 0).x;
            FragColor += (timeStep * (T - ambientTemperature) * sigma - D * kappa ) * vec2(0, 1);
        }
    }

#elif defined(DIVERGENCE_SHADER)

    out float FragColor;

    uniform sampler2D Velocity;
    uniform sampler2D Obstacles;
    uniform float HalfInverseCellSize;

    void main()
    {
        ivec2 T = ivec2(gl_FragCoord.xy);

        // Find neighboring velocities:
        vec2 vN = texelFetchOffset(Velocity, T, 0, ivec2(0, 1)).xy;
        vec2 vS = texelFetchOffset(Velocity, T, 0, ivec2(0, -1)).xy;
        vec2 vE = texelFetchOffset(Velocity, T, 0, ivec2(1, 0)).xy;
        vec2 vW = texelFetchOffset(Velocity, T, 0, ivec2(-1, 0)).xy;

        // Find neighboring obstacles:
        vec3 oN = texelFetchOffset(Obstacles, T, 0, ivec2(0, 1)).xyz;
        vec3 oS = texelFetchOffset(Obstacles, T, 0, ivec2(0, -1)).xyz;
        vec3 oE = texelFetchOffset(Obstacles, T, 0, ivec2(1, 0)).xyz;
        vec3 oW = texelFetchOffset(Obstacles, T, 0, ivec2(-1, 0)).xyz;

        // Use obstacle velocities for solid cells:
        if (oN.x > 0) vN = oN.yz;
        if (oS.x > 0) vS = oS.yz;
        if (oE.x > 0) vE = oE.yz;
        if (oW.x > 0) vW = oW.yz;

        FragColor = HalfInverseCellSize * (vE.x - vW.x + vN.y - vS.y);
    }



#elif defined(IMPULSE_SHADER)

    uniform float radius;
    uniform vec2 point;
    uniform vec3 fillColor;
    out vec4 FragColor;

    void main () {
        float d = distance(point, gl_FragCoord.xy);
        if (d < radius) {
            float a = (radius - d) * 0.5;
            a = min(a, 1.0);
            FragColor = vec4(fillColor, a);
        }
        else {
            FragColor = vec4(0);
        }
    }

#elif defined(OBSTACLE_SHADER)

    out vec4 FragColor;
    in vec2 TexCoord;
    uniform vec2 position;
    uniform float radius;
    uniform vec4 color;

    void main () {
        if (length(TexCoord - position) < radius) FragColor = color;
        else discard;
    }

#elif defined(VISUALIZE_SHADER)
    out vec4 FragColor;
    in vec2 TexCoord;
    uniform sampler2D image;
    uniform vec3 fillColor;
    //uniform vec2 scale;

    void main () {
        //float L = texture(image, gl_FragCoord.xy * scale).r;
        float L = texture(image, TexCoord).r;
        FragColor = vec4(fillColor, L);
    }

#endif
