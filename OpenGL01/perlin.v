#version 460 
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

out vec2 TexCoord;
out vec3 Pos;
out float time;

uniform mat4 transform;
uniform float offset;

float rand(float x){
    return fract(sin(x)*100000.0f);
}

float rand(vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = rand(i);
    float b = rand(i + vec2(1.0, 0.0));
    float c = rand(i + vec2(0.0, 1.0));
    float d = rand(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}


void main(){
    float n=noise(aPos.xz);
    gl_Position = transform * vec4(aPos, 1.0f);
    //gl_Position = transform * vec4(aPos.x,aPos.y+abs(offset-n),aPos.z, 1.0f);
    TexCoord=aTex;
    Pos=aPos;
    time=offset;
}