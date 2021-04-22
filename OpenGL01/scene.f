#version 460

in vec2 Tex;

out vec4 fragcolor;

uniform sampler2D Floor;

void main(){
    fragcolor = vec4(0.5f,0.5f,0.7f,1.0f);
    //贴图之后用分型噪声算一张
}